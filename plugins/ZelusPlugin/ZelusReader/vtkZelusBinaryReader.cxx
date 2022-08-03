#include "vtkZelusBinaryReader.h"

#include "vtkCellData.h"
#include "vtkCommand.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtksys/SystemTools.hxx"

#include "zsSimulationWorld.h"
#include "zsDeserializeBinary.h"

using namespace ZELUS;

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkZelusBinaryReader);

class vtkZelusBinaryReader::vtkInternals
{
public:
	vtkSmartPointer<vtkPolyData> BuildCurrentMesh(vtkPolyData* initialMesh)
	{
		vtkNew<vtkPolyData> output;

		const zsSimMesh& simMesh = this->SimWorld.GetSimMesh();
		const zsSimMapping& simMapping = this->SimWorld.GetSimMapping();

		this->SimWorld.UpdateEnergies();

		// data changing over time
		vtkNew<vtkPoints> points;
		points->SetData(this->ConvertArray<zsVector3, vtkFloatArray, 3>("position", simMesh.vertices.positions));
		output->SetPoints(points);

		// point data
		output->GetPointData()->SetNormals(this->ConvertArray<zsVector3, vtkFloatArray, 3>("normal", simMesh.perVertexNormalsStart));
		output->GetPointData()->SetScalars(this->ConvertArray<zsVector3, vtkFloatArray, 3>("velocity", simMesh.vertices.velocities));
		output->GetPointData()->AddArray(this->ConvertArray<zsReal, vtkFloatArray, 1>("energy", simMesh.vertices.energies));

		// fixed data over time
		if (initialMesh)
		{
			// shallow copy
			output->SetPolys(initialMesh->GetPolys());
			output->SetLines(initialMesh->GetLines());
			output->GetPointData()->SetTCoords(initialMesh->GetPointData()->GetTCoords());
			output->GetPointData()->AddArray(initialMesh->GetPointData()->GetArray("type"));
			output->GetPointData()->AddArray(initialMesh->GetPointData()->GetArray("invMass"));
			output->GetCellData()->AddArray(initialMesh->GetCellData()->GetArray("edgeColor"));
			output->GetCellData()->AddArray(initialMesh->GetCellData()->GetArray("triangleColor"));
		}
		else
		{
			// triangles
			vtkNew<vtkCellArray> triangles;
			for (const zsClothTriangle& t : simMesh.triangles)
			{
				triangles->InsertNextCell({ t.vertexIndices[0], t.vertexIndices[1], t.vertexIndices[2] });
			}
			output->SetPolys(triangles);

			// edges
			vtkNew<vtkCellArray> edges;
			for (const zsClothEdge& e : simMesh.edges)
			{
				edges->InsertNextCell({ e.vertexIndices[0], e.vertexIndices[1] });
			}
			output->SetLines(edges);

			// point data
			output->GetPointData()->SetTCoords(this->ConvertArray<zsVector2, vtkFloatArray, 2>("uv", simMesh.vertices.uvCoordinates));
			output->GetPointData()->AddArray(this->ConvertArray<ZS_SIMULATION_OBJECT_TYPE, vtkUnsignedCharArray, 1>("type", simMesh.simTypeVertices));
			output->GetPointData()->AddArray(this->ConvertArray<zsReal, vtkFloatArray, 1>("invMass", simMesh.vertices.inverseMasses));

			// cell data
			vtkNew<vtkIntArray> edgeColors;
			edgeColors->SetName("edgeColor");
			edgeColors->SetNumberOfTuples(simMesh.edges.size() + simMesh.triangles.size());
			edgeColors->FillValue(-1);
			simMapping.GetEdgeGlobalIndexMap().RunBatchArray([&](zsInt index, zsInt color) { edgeColors->SetTypedComponent(simMapping.ToGlobalEdge(index), 0, color); });

			vtkNew<vtkIntArray> triangleColors;
			triangleColors->SetName("triangleColor");
			triangleColors->SetNumberOfTuples(simMesh.edges.size() + simMesh.triangles.size());
			triangleColors->FillValue(-1);
			simMapping.GetTriangleGlobalIndexMap().RunBatchArray([&](zsInt index, zsInt color) { triangleColors->SetTypedComponent(simMesh.edges.size() + simMapping.ToGlobalTriangle(index), 0, color); });

			output->GetCellData()->AddArray(edgeColors);
			output->GetCellData()->AddArray(triangleColors);
		}

		return output;
	}

	void UpdateFrame(int frame)
	{
		for (int i = 1; i <= frame; i++)
		{
			if (this->Cache[i] == nullptr)
			{
				this->SimWorld.Update();
				this->Cache[i] = this->BuildCurrentMesh(this->Cache[0]);
			}
		}
	};

	template<typename inT, typename outT, int N>
	std::enable_if_t<N >= 2> AddTuple(outT* dataArray, inT elem, int index)
	{
		for (int j = 0; j < N; j++)
		{
			dataArray->SetTypedComponent(index, j, elem[j]);
		}
	}

	template<typename inT, typename outT, int N>
	std::enable_if_t<N == 1> AddTuple(outT* dataArray, inT elem, int index)
	{
		dataArray->SetTypedComponent(index, 0, elem);
	}

	template<typename inT, typename outT, int N>
	vtkSmartPointer<outT> ConvertArray(const char* name, const zsArray<inT>& buffer)
	{
		vtkNew<outT> dataArray;
		dataArray->SetNumberOfComponents(N);
		dataArray->SetName(name);
		dataArray->SetNumberOfTuples(buffer.size());

		for (int i = 0; i < buffer.size(); i++)
		{
			AddTuple<inT, outT, N>(dataArray.GetPointer(), buffer[i], i);
		}

		return dataArray;
	}

	std::vector<vtkSmartPointer<vtkPolyData>> Cache;
	zsSimulationWorld SimWorld;
};

//------------------------------------------------------------------------------
vtkZelusBinaryReader::vtkZelusBinaryReader()
{
	this->SetNumberOfInputPorts(0);
	this->Internals = new vtkInternals;
}

//------------------------------------------------------------------------------
vtkZelusBinaryReader::~vtkZelusBinaryReader()
{
	this->SetFileName(nullptr);
	delete this->Internals;
}

//------------------------------------------------------------------------------
void vtkZelusBinaryReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "FileName: " << (this->FileName ? this->FileName : "(none)") << "\n";
}

//------------------------------------------------------------------------------
int vtkZelusBinaryReader::RequestInformation(
	vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
	if (!this->Superclass::RequestInformation(request, inputVector, outputVector))
	{
		return 0;
	}

	if (!this->FileName)
	{
		vtkErrorMacro("A FileName must be specified.");
		return 0;
	}

	std::string fileNameAsString(this->FileName);

	if (fileNameAsString.find('\\') != std::string::npos)
	{
		vtksys::SystemTools::ConvertToUnixSlashes(fileNameAsString);
	}

	if (!vtksys::SystemTools::FileIsFullPath(fileNameAsString))
	{
		fileNameAsString = vtksys::SystemTools::CollapseFullPath(fileNameAsString);
	}

	if (this->FileName != fileNameAsString)
	{
		this->SetFileName(fileNameAsString.c_str());
	}

	// Read the file and load initial state
	zsDeserializeBinary(this->Internals->SimWorld, this->FileName);

	this->Internals->SimWorld.GetSimWorldDescriptor().deterministic = true;
	zsReal timestep = this->Internals->SimWorld.GetPhysicsDescriptor().timestep;

	zsInt maxFrame = this->MaxTime * timestep;

	vtkInformation* info = outputVector->GetInformationObject(0);

	if (info->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()))
	{
		info->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
	}
	this->Internals->Cache.clear();

	for (int i = 0; i <= maxFrame; i++)
	{
		info->Append(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), i / timestep);
		this->Internals->Cache.push_back(nullptr);
	}

	std::array<double, 2> timeRange = { { 0.0, this->MaxTime } };
	info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange.data(), 2);

	// convert initial state
	this->Internals->Cache[0] = this->Internals->BuildCurrentMesh(nullptr);

	return 1;
}

//------------------------------------------------------------------------------
int vtkZelusBinaryReader::RequestData(
	vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
	// Get the output
	vtkPolyData* output = vtkPolyData::GetData(outputVector);

	// Apply selected animations on specified time step to the model's transforms
	vtkInformation* info = outputVector->GetInformationObject(0);

	double time = info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
	int frameIndex = time * this->Internals->SimWorld.GetPhysicsDescriptor().timestep;

	this->Internals->UpdateFrame(frameIndex);
	output->ShallowCopy(this->Internals->Cache[frameIndex]);

	return 1;
}

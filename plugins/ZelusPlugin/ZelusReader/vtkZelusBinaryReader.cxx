#include "vtkZelusBinaryReader.h"

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
	vtkSmartPointer<vtkPolyData> BuildCurrentMesh()
	{
		vtkNew<vtkPolyData> output;

		const zsSimMesh& simMesh = this->SimWorld.GetSimMesh();

		// vertices
		vtkNew<vtkPoints> points;
		points->SetData(this->ConvertArray(simMesh.vertices.positions));
		output->SetPoints(points);

		// triangles
		vtkNew<vtkCellArray> triangles;
		for (const zsClothTriangle& t : simMesh.triangles)
		{
			triangles->InsertNextCell({ t.vertexIndices[0], t.vertexIndices[1], t.vertexIndices[2] });
		}
		output->SetPolys(triangles);

		return output;
	}

	void UpdateFrame(int frame)
	{
		for (int i = 1; i <= frame; i++)
		{
			if (this->Cache[i] == nullptr)
			{
				this->SimWorld.Update();
				this->Cache[i] = this->BuildCurrentMesh();
			}
		}
	};

	vtkSmartPointer<vtkFloatArray> ConvertArray(const zsArray<zsVector3>& buffer)
	{
		vtkNew<vtkFloatArray> dataArray;
		dataArray->SetNumberOfComponents(3);
		dataArray->SetNumberOfTuples(buffer.size());

		for (int i = 0; i < buffer.size(); i++)
		{
			dataArray->SetTypedComponent(i, 0, buffer[i].x);
			dataArray->SetTypedComponent(i, 1, buffer[i].y);
			dataArray->SetTypedComponent(i, 2, buffer[i].z);
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
	this->Internals->Cache[0] = this->Internals->BuildCurrentMesh();

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

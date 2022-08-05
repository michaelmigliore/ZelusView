#include "vtkZelusExtractType.h"

#include "vtkCommand.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkThreshold.h"
#include "vtkDataSetSurfaceFilter.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkZelusExtractType);

//------------------------------------------------------------------------------
vtkZelusExtractType::vtkZelusExtractType() = default;

//------------------------------------------------------------------------------
vtkZelusExtractType::~vtkZelusExtractType() = default;

//------------------------------------------------------------------------------
void vtkZelusExtractType::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "ExtractType: " << this->ClothType << "\n";
}

//------------------------------------------------------------------------------
int vtkZelusExtractType::RequestData(
	vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
	// Get the output
	vtkPolyData* output = vtkPolyData::GetData(outputVector);

	// threshold by type
	vtkNew<vtkThreshold> threshold;
	threshold->SetInputData(this->GetInput());
	threshold->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "type");
	threshold->SetThresholdFunction(this->ClothType != 0 ? vtkThreshold::THRESHOLD_LOWER : vtkThreshold::THRESHOLD_UPPER);
	threshold->SetUpperThreshold(1.5);
	threshold->SetLowerThreshold(1.5);

	// convert UG back to polydata
	vtkNew<vtkDataSetSurfaceFilter> surface;
	surface->SetInputConnection(threshold->GetOutputPort());
	surface->Update();

	output->ShallowCopy(surface->GetOutput());

	return 1;
}

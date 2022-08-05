#include "vtkZelusExtractCell.h"

#include "vtkCommand.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkThreshold.h"
#include "vtkDataSetSurfaceFilter.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkZelusExtractCell);

//------------------------------------------------------------------------------
vtkZelusExtractCell::vtkZelusExtractCell() = default;

//------------------------------------------------------------------------------
vtkZelusExtractCell::~vtkZelusExtractCell() = default;

//------------------------------------------------------------------------------
void vtkZelusExtractCell::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "CellType: " << this->CellType << "\n";
}

//------------------------------------------------------------------------------
int vtkZelusExtractCell::RequestData(
	vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
	// Get the output
	vtkPolyData* output = vtkPolyData::GetData(outputVector);

	output->ShallowCopy(this->GetInput());

	if (this->CellType == 0)
	{
		output->SetLines(nullptr);
	}
	else
	{
		output->SetPolys(nullptr);
	}

	return 1;
}

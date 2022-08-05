#pragma once

#include "vtkPolyDataAlgorithm.h"

class vtkZelusExtractCell : public vtkPolyDataAlgorithm
{
public:
	static vtkZelusExtractCell* New();
	vtkTypeMacro(vtkZelusExtractCell, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	vtkSetMacro(CellType, int);
	vtkGetMacro(CellType, int);

protected:
	vtkZelusExtractCell();
	~vtkZelusExtractCell() override;

	int CellType = 0;

	int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
	vtkZelusExtractCell(const vtkZelusExtractCell&) = delete;
	void operator=(const vtkZelusExtractCell&) = delete;

	class vtkInternals;
	vtkInternals* Internals;
};


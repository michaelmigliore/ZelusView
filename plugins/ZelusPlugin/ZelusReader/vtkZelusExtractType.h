#pragma once

#include "vtkPolyDataAlgorithm.h"

class vtkZelusExtractType : public vtkPolyDataAlgorithm
{
public:
	static vtkZelusExtractType* New();
	vtkTypeMacro(vtkZelusExtractType, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	vtkSetMacro(ClothType, int);
	vtkGetMacro(ClothType, int);

protected:
	vtkZelusExtractType();
	~vtkZelusExtractType() override;

	int ClothType = 0;

	int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
	vtkZelusExtractType(const vtkZelusExtractType&) = delete;
	void operator=(const vtkZelusExtractType&) = delete;

	class vtkInternals;
	vtkInternals* Internals;
};


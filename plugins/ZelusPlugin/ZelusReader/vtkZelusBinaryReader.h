#pragma once

#include "vtkPolyDataAlgorithm.h"

class vtkZelusBinaryReader : public vtkPolyDataAlgorithm
{
public:
	static vtkZelusBinaryReader* New();
	vtkTypeMacro(vtkZelusBinaryReader, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	vtkSetMacro(MaxTime, double);
	vtkGetMacro(MaxTime, double);

	vtkSetMacro(TimeStep, double);
	vtkGetMacro(TimeStep, double);

protected:
	vtkZelusBinaryReader();
	~vtkZelusBinaryReader() override;

	char* FileName = nullptr;
	double MaxTime = 0.0;
	double TimeStep = 30.0;

	int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
	int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
	vtkZelusBinaryReader(const vtkZelusBinaryReader&) = delete;
	void operator=(const vtkZelusBinaryReader&) = delete;

	class vtkInternals;
	vtkInternals* Internals;
};


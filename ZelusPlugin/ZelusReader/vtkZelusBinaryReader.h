#pragma once

#include "vtkMultiBlockDataSetAlgorithm.h"

class vtkZelusBinaryReader : public vtkMultiBlockDataSetAlgorithm
{
public:
	static vtkZelusBinaryReader* New();
	vtkTypeMacro(vtkZelusBinaryReader, vtkMultiBlockDataSetAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	vtkSetMacro(MaxTime, double);
	vtkGetMacro(MaxTime, double);

protected:
	vtkZelusBinaryReader();
	~vtkZelusBinaryReader() override;

	char* FileName = nullptr;
	double MaxTime = 0.0;

	int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
	int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
	vtkZelusBinaryReader(const vtkZelusBinaryReader&) = delete;
	void operator=(const vtkZelusBinaryReader&) = delete;

	class vtkInternals;
	vtkInternals* Internals;
};


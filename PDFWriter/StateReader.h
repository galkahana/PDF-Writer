#pragma once

#include "EStatusCode.h"
#include "InputFile.h"
#include "ObjectsBasicTypes.h"
#include "PDFParser.h"

#include <string>

using namespace std;

class StateReader
{
public:
	StateReader(void);
	~StateReader(void);

	EStatusCode Start(const wstring& inStateFilePath);
	PDFParser* GetObjectsReader();
	ObjectIDType GetRootObjectID();
	void Finish();

private:

	PDFParser mParser;
	InputFile mInputFile;
	ObjectIDType mRootObject;
};

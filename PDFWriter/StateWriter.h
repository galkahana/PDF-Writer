#pragma once

#include "EStatusCode.h"
#include "OutputFile.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"

#include <string>

using namespace std;

class ObjectsContext;

class StateWriter
{
public:
	StateWriter(void);
	virtual ~StateWriter(void);

	EStatusCode Start(const wstring& inStateFilePath);
	ObjectsContext* GetObjectsWriter();
	void SetRootObject(ObjectIDType inRootObjectID);
	EStatusCode Finish();

private:

	ObjectsContext* mObjectsContext;
	OutputFile mOutputFile;
	ObjectIDType mRootObject;

	void WriteTrailerDictionary();
	void WriteXrefReference(IOBasicTypes::LongFilePositionType inXrefTablePosition);
	void WriteFinalEOF();

};

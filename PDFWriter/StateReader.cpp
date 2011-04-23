#include "StateReader.h"
#include "PDFObjectCast.h"
#include "PDFIndirectObjectReference.h"
#include "Trace.h"

StateReader::StateReader(void)
{
}

StateReader::~StateReader(void)
{
}

EStatusCode StateReader::Start(const wstring& inStateFilePath)
{
	// open the new file...
	if(mInputFile.OpenFile(inStateFilePath) != eSuccess)
	{
		TRACE_LOG1("StateReader::Start, can't open file for state reading in %s",inStateFilePath.c_str());
		return eFailure;
	}
	
	if(mParser.StartStateFileParsing(mInputFile.GetInputStream()) != eSuccess)
	{
		TRACE_LOG("StateReader::Start, unable to start parsing for the state reader file");
		return eFailure;
	}

	// set the root object
	PDFObjectCastPtr<PDFIndirectObjectReference> rootObject(mParser.GetTrailer()->QueryDirectObject("Root"));
	mRootObject = rootObject->mObjectID;

	return eSuccess;
}

PDFParser* StateReader::GetObjectsReader()
{
	return &mParser;

}

ObjectIDType StateReader::GetRootObjectID()
{
	return mRootObject;
}

void StateReader::Finish()
{
	mParser.ResetParser();
}

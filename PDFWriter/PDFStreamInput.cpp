#include "PDFStreamInput.h"
#include "PDFDictionary.h"
#include "PDFName.h"

PDFStreamInput::PDFStreamInput(PDFDictionary* inStreamDictionary,LongFilePositionType inStreamContentStart) : PDFObject(eType)
{
	mDictionary = inStreamDictionary;
	mStreamContentStart = inStreamContentStart;
}

PDFStreamInput::~PDFStreamInput(void)
{
	mDictionary->Release();
}

PDFDictionary* PDFStreamInput::QueryStreamDictionary()
{
	mDictionary->AddRef();
	return mDictionary;
}

LongFilePositionType PDFStreamInput::GetStreamContentStart()
{
	return mStreamContentStart;
}


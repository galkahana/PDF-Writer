#include "PDFStreamInput.h"
#include "PDFDictionary.h"
#include "PDFName.h"

PDFStreamInput::PDFStreamInput(PDFDictionary* inStreamDictionary,LongFilePositionType inStreamContentStart) : PDFObject(ePDFObjectStream)
{
	mDictionary = inStreamDictionary;
	mStreamContentStart = inStreamContentStart;
}

PDFStreamInput::~PDFStreamInput(void)
{
	delete mDictionary;
}

PDFDictionary* PDFStreamInput::GetStreamDictionary()
{
	return mDictionary;
}

PDFObject* PDFStreamInput::GetExtentObject()
{
	PDFName nameLength("Length");

	PDFNameToPDFObjectMap::const_iterator it = (*mDictionary)->find(&nameLength);
	if(it == (*mDictionary)->end())
		return NULL; // k. an exception
	else
		return it->second;
}

LongFilePositionType PDFStreamInput::GetStreamContentStart()
{
	return mStreamContentStart;
}


#pragma once

#include "RefCountObject.h"

#include <string>

using namespace std;

enum EPDFObjectType
{
	ePDFObjectBoolean,
	ePDFObjectLiteralString,
	ePDFObjectHexString,
	ePDFObjectNull,
	ePDFObjectName,
	ePDFObjectInteger,
	ePDFObjectReal,
	ePDFObjectArray,
	ePDFObjectDictionary,
	ePDFObjectIndirectObjectReference,
	ePDFObjectStream,
	ePDFObjectSymbol // symbol is parallel to unkown. mostly be a keyword, or simply a mistake in the file
};

static const char* scPDFObjectTypeLabel[] = 
{
	"Boolean",
	"LiteralString",
	"HexString",
	"Null",
	"Name",
	"Integer",
	"Real",
	"Array",
	"Dictionary",
	"IndirectObjectReference",
	"Stream",
	"Symbol"
};

class PDFObject : public RefCountObject
{
public:
	PDFObject(EPDFObjectType inType); 
	PDFObject(int inType); 
	virtual ~PDFObject(void);

	EPDFObjectType GetType();

private:
	EPDFObjectType mType;
};

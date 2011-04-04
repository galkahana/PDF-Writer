#pragma once
#include <string>

using namespace std;

enum ePDFObjectType
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

class PDFObject
{
public:
	PDFObject(ePDFObjectType inType);
	virtual ~PDFObject(void);

	ePDFObjectType GetType();

private:
	ePDFObjectType mType;
};

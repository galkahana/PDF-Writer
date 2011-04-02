#pragma once


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

class PDFObject
{
public:
	PDFObject(ePDFObjectType inType);
	virtual ~PDFObject(void);

	ePDFObjectType GetType();

private:
	ePDFObjectType mType;
};

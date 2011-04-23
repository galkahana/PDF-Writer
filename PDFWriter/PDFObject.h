/*
   Source File : PDFObject.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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

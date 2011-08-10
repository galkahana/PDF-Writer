/*
   Source File : CFFANSIFontWriter.cpp


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
#include "CFFANSIFontWriter.h"
#include "ANSIFontWriter.h"
#include "DictionaryContext.h"
#include "IByteWriterWithPosition.h"
#include "PrimitiveObjectsWriter.h"
#include "PDFStream.h"
#include "SafeBufferMacrosDefs.h"
#include "ObjectsContext.h"
#include "FreeTypeFaceWrapper.h"
#include "CFFEmbeddedFontWriter.h"
#include "Type1ToCFFEmbeddedFontWriter.h"
#include "Trace.h"

#include <ft2build.h>
#include FT_FREETYPE_H

CFFANSIFontWriter::CFFANSIFontWriter(void)
{
}

CFFANSIFontWriter::~CFFANSIFontWriter(void)
{
}

static const string scType1C = "Type1C";
static const char* scType1Type = "Type 1";
static const char* scCFF = "CFF";
static const string scPlus = "+";
EPDFStatusCode CFFANSIFontWriter::WriteFont(	FreeTypeFaceWrapper& inFontInfo,
											WrittenFontRepresentation* inFontOccurrence,
											ObjectsContext* inObjectsContext)
{
	const char* postscriptFontName = FT_Get_Postscript_Name(inFontInfo);
	if(!postscriptFontName)
	{
		TRACE_LOG("CFFANSIFontWriter::WriteFont, unexpected failure. no postscript font name for font");
		return ePDFFailure;
	}
	std::string subsetFontName = inObjectsContext->GenerateSubsetFontPrefix() + scPlus + postscriptFontName;
	
	const char* fontType = inFontInfo.GetTypeString();

	// reset embedded font object ID (and flag...to whether it was actually embedded or not, which may 
	// happen due to font embedding restrictions)
	mEmbeddedFontFileObjectID = 0;

	EPDFStatusCode status;
	if(strcmp(scType1Type,fontType) == 0)
	{
		Type1ToCFFEmbeddedFontWriter embeddedFontWriter;

		status = embeddedFontWriter.WriteEmbeddedFont(inFontInfo,
													inFontOccurrence->GetGlyphIDsAsOrderedVector(),
													scType1C,
													subsetFontName, 
													inObjectsContext,
													mEmbeddedFontFileObjectID);
	}
	else if(strcmp(scCFF,fontType) == 0)
	{
		CFFEmbeddedFontWriter embeddedFontWriter;

		status = embeddedFontWriter.WriteEmbeddedFont(inFontInfo,
													inFontOccurrence->GetGlyphIDsAsOrderedVector(),
													scType1C,
													subsetFontName, 
													inObjectsContext,
													mEmbeddedFontFileObjectID);
	}
	else
	{

		TRACE_LOG("CFFANSIFontWriter::WriteFont, Exception, unfamilar font type for embedding representation");
		status = ePDFFailure;
	}
	if(status != ePDFSuccess)
		return status;

	ANSIFontWriter fontWriter;

	return fontWriter.WriteFont(inFontInfo,inFontOccurrence,inObjectsContext,this,subsetFontName);
}

static const char* scType1 = "Type1";
void CFFANSIFontWriter::WriteSubTypeValue(DictionaryContext* inDictionary)
{
	inDictionary->WriteNameValue(scType1);
}

IFontDescriptorHelper* CFFANSIFontWriter::GetCharsetWriter()
{
	return this;
}


static const string scCharSet = "CharSet";
static const Byte scLeftParanthesis[] = {'('};
static const Byte scRightParanthesis[] = {')'};

void CFFANSIFontWriter::WriteCharSet(	DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext,
										FreeTypeFaceWrapper* inFontInfo,
										const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs)
{
	// ChartSet
	inDescriptorContext->WriteKey(scCharSet);

	// charset is a rather long string. i'd rather write it down as it goes, than format a string
	// and submit it.
	IByteWriterWithPosition* directStream = inObjectsContext->StartFreeContext();
	PrimitiveObjectsWriter primitiveWriter(directStream);

	directStream->Write(scLeftParanthesis,1);

	UIntAndGlyphEncodingInfoVector::const_iterator it = inEncodedGlyphs.begin()+1; // skip 0 character
	char buffer[100];
	
	for(; it != inEncodedGlyphs.end(); ++it)
	{
		FT_Get_Glyph_Name(*inFontInfo,it->first,buffer,100);
		primitiveWriter.WriteName(buffer,eTokenSepratorNone);
	}
	directStream->Write(scRightParanthesis,1);
	inObjectsContext->EndFreeContext();
	inObjectsContext->EndLine();
}

static const string scFontFile3 = "FontFile3";
void CFFANSIFontWriter::WriteFontFileReference(	
										DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext)
{
	// write font reference only if there's what to write....
	if(mEmbeddedFontFileObjectID != 0)
	{
		// FontFile3
		inDescriptorContext->WriteKey(scFontFile3);
		inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
	}
}

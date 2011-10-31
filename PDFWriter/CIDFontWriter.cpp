/*
   Source File : CIDFontWriter.cpp


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
#include "CIDFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "FreeTypeFaceWrapper.h"
#include "Trace.h"
#include "PDFStream.h"
#include "IByteWriter.h"
#include "SafeBufferMacrosDefs.h"
#include "CFFDescendentFontWriter.h"
#include "IDescendentFontWriter.h"
#include "UnicodeString.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>

using namespace PDFHummus;

CIDFontWriter::CIDFontWriter(void)
{
}

CIDFontWriter::~CIDFontWriter(void)
{
}

static const string scType = "Type";
static const string scFont = "Font";
static const string scSubtype = "Subtype";
static const string scType0 = "Type0";
static const string scBaseFont = "BaseFont";
static const string scPlus = "+";
static const string scDescendantFonts = "DescendantFonts";
static const string scToUnicode = "ToUnicode";

EStatusCode CIDFontWriter::WriteFont(FreeTypeFaceWrapper& inFontInfo,
										WrittenFontRepresentation* inFontOccurrence,
										ObjectsContext* inObjectsContext,
										IDescendentFontWriter* inDescendentFontWriter)
{

	EStatusCode status = PDFHummus::eSuccess;
	inObjectsContext->StartNewIndirectObject(inFontOccurrence->mWrittenObjectID);

	mFontInfo = &inFontInfo;
	mFontOccurrence = inFontOccurrence;
	mObjectsContext = inObjectsContext;

	do
	{
		DictionaryContext* fontContext = inObjectsContext->StartDictionary();

		// Type
		fontContext->WriteKey(scType);
		fontContext->WriteNameValue(scFont);

		// SubType
		fontContext->WriteKey(scSubtype);
		fontContext->WriteNameValue(scType0);

		// BaseFont
		fontContext->WriteKey(scBaseFont);
		const char* postscriptFontName = FT_Get_Postscript_Name(inFontInfo);
		if(!postscriptFontName)
		{
			TRACE_LOG("CIDFontWriter::WriteFont, unexpected failure. no postscript font name for font");
			status = PDFHummus::eFailure;
			break;
		}
		std::string subsetFontName = inObjectsContext->GenerateSubsetFontPrefix() + scPlus + postscriptFontName;
		fontContext->WriteNameValue(subsetFontName);

		WriteEncoding(fontContext);

		// DescendantFonts
		ObjectIDType descendantFontID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

		fontContext->WriteKey(scDescendantFonts);
		mObjectsContext->StartArray();
		mObjectsContext->WriteIndirectObjectReference(descendantFontID);
		mObjectsContext->EndArray(eTokenSeparatorEndLine);

		CalculateCharacterEncodingArray(); // put the charachter in the order of encoding, for the ToUnicode map

		// ToUnicode
		fontContext->WriteKey(scToUnicode);
		ObjectIDType toUnicodeMapObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
		fontContext->WriteObjectReferenceValue(toUnicodeMapObjectID);

		status = inObjectsContext->EndDictionary(fontContext);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("CIDFontWriter::WriteFont, unexpected failure. Failed to end dictionary in font write.");
			break;
		}
		inObjectsContext->EndIndirectObject();
		WriteToUnicodeMap(toUnicodeMapObjectID);

		// Write the descendant font
		status = inDescendentFontWriter->WriteFont(descendantFontID,subsetFontName,*mFontInfo,mCharactersVector,mObjectsContext);

	} while(false);

	return status;
}

static const string scEncoding = "Encoding";
static const string scIdentityH = "Identity-H";
void CIDFontWriter::WriteEncoding(DictionaryContext* inFontContext)
{
	// Encoding
	inFontContext->WriteKey(scEncoding);
	inFontContext->WriteNameValue(scIdentityH);
}

static bool sUShortSort(const UIntAndGlyphEncodingInfo& inLeft, const UIntAndGlyphEncodingInfo& inRight)
{
	return inLeft.second.mEncodedCharacter < inRight.second.mEncodedCharacter;
}

void CIDFontWriter::CalculateCharacterEncodingArray()
{
	// first we need to sort the fonts charachters by character code
	UIntToGlyphEncodingInfoMap::iterator it = mFontOccurrence->mGlyphIDToEncodedChar.begin();

	for(; it != mFontOccurrence->mGlyphIDToEncodedChar.end();++it)
		mCharactersVector.push_back(UIntAndGlyphEncodingInfo(it->first,it->second));

	std::sort(mCharactersVector.begin(),mCharactersVector.end(),sUShortSort);
}


static const char* scCmapHeader =
"/CIDInit /ProcSet findresource begin\n\
12 dict begin\n\
begincmap\n\
/CIDSystemInfo\n\
<< /Registry (Adobe)\n\
/Ordering (UCS) /Supplement 0 >> def\n\
/CMapName /Adobe-Identity-UCS def\n\
/CMapType 2 def\n\
1 begincodespacerange\n";
static const char* scFourByteRangeStart = "0000";
static const char* scFourByteRangeEnd = "FFFF";
static const char* scEndCodeSpaceRange = "endcodespacerange\n";
static const string scBeginBFChar = "beginbfchar";
static const string scEndBFChar = "endbfchar";
static const char* scCmapFooter = "endcmap CMapName currentdict /CMap defineresource pop end end\n";

void CIDFontWriter::WriteToUnicodeMap(ObjectIDType inToUnicodeMap)
{
	mObjectsContext->StartNewIndirectObject(inToUnicodeMap);
	PDFStream* pdfStream = mObjectsContext->StartPDFStream();
	IByteWriter* cmapWriteContext = pdfStream->GetWriteStream();
	PrimitiveObjectsWriter primitiveWriter(cmapWriteContext);
	unsigned long i = 1;
	UIntAndGlyphEncodingInfoVector::iterator it = mCharactersVector.begin() + 1; // skip 0 glyph
	unsigned long vectorSize = (unsigned long)mCharactersVector.size() - 1; // cause 0 is not there

	cmapWriteContext->Write((const Byte*)scCmapHeader,strlen(scCmapHeader));
	primitiveWriter.WriteHexString(scFourByteRangeStart);
	primitiveWriter.WriteHexString(scFourByteRangeEnd,eTokenSeparatorEndLine);
	cmapWriteContext->Write((const Byte*)scEndCodeSpaceRange,strlen(scEndCodeSpaceRange));

	if(vectorSize < 100)
		primitiveWriter.WriteInteger(vectorSize);
	else
		primitiveWriter.WriteInteger(100);
	primitiveWriter.WriteKeyword(scBeginBFChar);

	WriteGlyphEntry(cmapWriteContext,it->second.mEncodedCharacter,it->second.mUnicodeCharacters);
	++it;
	for(; it != mCharactersVector.end(); ++it,++i)
	{
		if(i % 100 == 0)
		{
			primitiveWriter.WriteKeyword(scEndBFChar);
			if(vectorSize - i < 100)
				primitiveWriter.WriteInteger(vectorSize - i);
			else
				primitiveWriter.WriteInteger(100);
			primitiveWriter.WriteKeyword(scBeginBFChar);
		}
		WriteGlyphEntry(cmapWriteContext,it->second.mEncodedCharacter,it->second.mUnicodeCharacters);
	}
	primitiveWriter.WriteKeyword(scEndBFChar);
	cmapWriteContext->Write((const Byte*)scCmapFooter,strlen(scCmapFooter));
	mObjectsContext->EndPDFStream(pdfStream);
	delete pdfStream;
}

static const Byte scEntryEnding[2] = {'>','\n'};
static const Byte scAllZeros[4] = {'0','0','0','0'};
void CIDFontWriter::WriteGlyphEntry(IByteWriter* inWriter,unsigned short inEncodedCharacter,const ULongVector& inUnicodeValues)
{
	UnicodeString unicode;
	char formattingBuffer[17];
	ULongVector::const_iterator it = inUnicodeValues.begin();

	SAFE_SPRINTF_1(formattingBuffer,17,"<%04x> <",inEncodedCharacter);
	inWriter->Write((const Byte*)formattingBuffer,8);

	if(inUnicodeValues.size() == 0)
	{
		inWriter->Write(scAllZeros,4);
	}
	else
	{
		for(; it != inUnicodeValues.end(); ++it)
		{
			unicode.GetUnicodeList().push_back(*it);
			EStatusCodeAndUShortList utf16Result = unicode.ToUTF16UShort();
			unicode.GetUnicodeList().clear();

			if(utf16Result.second.size() == 2)
			{
				SAFE_SPRINTF_2(formattingBuffer,17,"%04x%04x",
																utf16Result.second.front(),
																utf16Result.second.back());
				inWriter->Write((const Byte*)formattingBuffer,8);
			}
			else // 1
			{
				SAFE_SPRINTF_1(formattingBuffer,17,"%04x",utf16Result.second.front());
				inWriter->Write((const Byte*)formattingBuffer,4);
			}
		}
	}
	inWriter->Write(scEntryEnding,2);
}

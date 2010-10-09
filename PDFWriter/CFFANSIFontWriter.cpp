#include "CFFANSIFontWriter.h"
#include "ObjectsContext.h"
#include "FreeTypeFaceWrapper.h"
#include "DictionaryContext.h"
#include "WrittenFontRepresentation.h"
#include "Trace.h"
#include "WinAnsiEncoding.h"
#include "IByteWriter.h"
#include "PrimitiveObjectsWriter.h"
#include "PDFStream.h"
#include "SafeBufferMacrosDefs.h"
#include "EFontStretch.h"
#include "IByteWriterWithPosition.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>

using namespace std;

CFFANSIFontWriter::CFFANSIFontWriter(void)
{
}

CFFANSIFontWriter::~CFFANSIFontWriter(void)
{
}

static const string scType = "Type";
static const string scFont = "Font";
static const string scSubtype = "Subtype";
static const string scType1 = "Type1";
static const string scBaseFont = "BaseFont";
static const string scPlus = "+";
static const string scToUnicode = "ToUnicode";
static const string scFontDescriptor = "FontDescriptor";

EStatusCode CFFANSIFontWriter::WriteFont(	FreeTypeFaceWrapper& inFontInfo,
											WrittenFontRepresentation* inFontOccurrence,
											ObjectsContext* inObjectsContext)
{
	EStatusCode status = eSuccess;
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
		fontContext->WriteNameValue(scType1);

		// BaseFont
		fontContext->WriteKey(scBaseFont);
		const char* postscriptFontName = FT_Get_Postscript_Name(inFontInfo);
		if(!postscriptFontName)
		{
			TRACE_LOG("CFFANSIFontWriter::WriteFont, unexpected failure. no postscript font name for font");
			status = eFailure;
			break;
		}
		std::string subsetFontName = inObjectsContext->GenerateSubsetFontPrefix() + scPlus + postscriptFontName;
		fontContext->WriteNameValue(subsetFontName);


		/*
			as for widths.
			i have to create a list/array of the characters ordered from lowest encoded value to highest, and fill it up with the charachters mapped
			to glyphs. this will allow me to later write the highest and lowest char codes, as well as loop the list and 
			write the widths.
		*/
		CalculateCharacterEncodingArray();

		WriteWidths(fontContext);

		CalculateDifferences();

		WriteEncoding(fontContext);

		// ToUnicode
		fontContext->WriteKey(scToUnicode);
		ObjectIDType toUnicodeMapObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
		fontContext->WriteObjectReferenceValue(toUnicodeMapObjectID);


		// FontDescriptor
		fontContext->WriteKey(scFontDescriptor);
		ObjectIDType fontDescriptorObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
		fontContext->WriteObjectReferenceValue(fontDescriptorObjectID);

		status = inObjectsContext->EndDictionary(fontContext);
		if(status != eSuccess)
		{
			TRACE_LOG("CFFANSIFontWriter::WriteFont, unexpected faiulre. Failed to end dictionary in font write.");
			break;
		}

		inObjectsContext->EndIndirectObject();	

		// if necessary, write a dictionary encoding
		if(mDifferences.size() > 0)
			WriteEncodingDictionary();

		WriteToUnicodeMap(toUnicodeMapObjectID);
		WriteFontDescriptor(fontDescriptorObjectID,subsetFontName);
	}while(false);
	return status;
}

static bool sUShortSort(const UIntAndGlyphEncodingInfo& inLeft, const UIntAndGlyphEncodingInfo& inRight)
{
	return inLeft.second.mEncodedCharacter < inRight.second.mEncodedCharacter;
}

void CFFANSIFontWriter::CalculateCharacterEncodingArray()
{
	// first we need to sort the fonts charachters by character code
	UIntToGlyphEncodingInfoMap::iterator it = mFontOccurrence->mGlyphIDToEncodedChar.begin();

	for(; it != mFontOccurrence->mGlyphIDToEncodedChar.end();++it)
		mCharactersVector.push_back(UIntAndGlyphEncodingInfo(it->first,it->second));

	std::sort(mCharactersVector.begin(),mCharactersVector.end(),sUShortSort);
}

static const string scFirstChar = "FirstChar";
static const string scLastChar = "LastChar";
static const string scWidths = "Widths";

void CFFANSIFontWriter::WriteWidths(DictionaryContext* inFontContext)
{

	// FirstChar
	inFontContext->WriteKey(scFirstChar);
	inFontContext->WriteIntegerValue((mCharactersVector.begin()+1)->second.mEncodedCharacter); // skip the 0 glyph when writing widths

	// LastChar
	inFontContext->WriteKey(scLastChar);
	inFontContext->WriteIntegerValue(mCharactersVector.back().second.mEncodedCharacter);

	// Widths
	inFontContext->WriteKey(scWidths);

	mObjectsContext->StartArray();
	
	UIntAndGlyphEncodingInfoVector::iterator itCharacters = mCharactersVector.begin() + 1; // ignore the 0 glyph when writing widths
	for(unsigned short i = itCharacters->second.mEncodedCharacter; i <= mCharactersVector.back().second.mEncodedCharacter; ++i)
	{
		if(itCharacters->second.mEncodedCharacter == i)
		{
			FT_Load_Glyph(*mFontInfo,itCharacters->first,FT_LOAD_NO_SCALE);
			mObjectsContext->WriteInteger((*mFontInfo)->glyph->metrics.width);
			++itCharacters;
		}
		else
		{
			mObjectsContext->WriteInteger(0);
		}
	}
	mObjectsContext->EndArray();
	mObjectsContext->EndLine();

}

void CFFANSIFontWriter::CalculateDifferences()
{
	// go over the encoded charachters. find differences from WinAnsiEncoding.
	// whenever glyph name is different, add to differences array
	WinAnsiEncoding winAnsiEncoding;
	char buffer[100];

	UIntAndGlyphEncodingInfoVector::iterator it = mCharactersVector.begin();

	for(; it != mCharactersVector.end(); ++it)
	{
		// hmm. there should always be a glyph name for a CFF or Type1 glyph. so it shouldn't be a problem
		// to ask them here. should be sufficient length as well.
		FT_Get_Glyph_Name(*mFontInfo,it->first,buffer,100);
		if(strcmp(buffer,winAnsiEncoding.GetEncodedGlyphName((IOBasicTypes::Byte)it->second.mEncodedCharacter)) != 0)
			mDifferences.push_back(UShortAndString(it->second.mEncodedCharacter,buffer));
	}
}

static const string scEncoding = "Encoding";
static const string scWinAnsiEncoding = "WinAnsiEncoding";
void CFFANSIFontWriter::WriteEncoding(DictionaryContext* inFontContext)
{
	// Encoding

	inFontContext->WriteKey(scEncoding);
	if(mDifferences.size() == 0)
		inFontContext->WriteNameValue(scWinAnsiEncoding);
	else	
		mEncodingDictionaryID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
}

static const string scBaseEncoding = "BaseEncoding";
static const string scDifferences = "Differences";
void CFFANSIFontWriter::WriteEncodingDictionary()
{
	DictionaryContext* encodingDictionary;

	mObjectsContext->StartNewIndirectObject(mEncodingDictionaryID);
	encodingDictionary = mObjectsContext->StartDictionary();

	// Type
	encodingDictionary->WriteKey(scType);
	encodingDictionary->WriteNameValue(scEncoding);

	// BaseEncoding
	encodingDictionary->WriteKey(scBaseEncoding);
	encodingDictionary->WriteNameValue(scWinAnsiEncoding);

	// Differences
	encodingDictionary->WriteKey(scDifferences);
	mObjectsContext->StartArray();

	UShortAndStringList::iterator it = mDifferences.begin();
	unsigned short previousEncoding;
	
	encodingDictionary->WriteIntegerValue(it->first);
	encodingDictionary->WriteNameValue(it->second);
	previousEncoding = it->first;
	++it;
	for(; it != mDifferences.end();++it)
	{
		if(previousEncoding + 1 != it->first)
		{
			mObjectsContext->EndLine();
			encodingDictionary->WriteIndents();
			encodingDictionary->WriteIntegerValue(it->first);
			encodingDictionary->WriteNameValue(it->second);

			// put two spaces to account for the starting bracket and following spaces. y'know, just for the sake of proper formatting
			mObjectsContext->WriteTokenSeparator(eTokenSeparatorSpace);
			mObjectsContext->WriteTokenSeparator(eTokenSeparatorSpace);
		}
		encodingDictionary->WriteNameValue(it->second);
		previousEncoding = it->first;
	}

	mObjectsContext->EndArray();
	mObjectsContext->EndLine();

	mObjectsContext->EndDictionary(encodingDictionary);
	mObjectsContext->EndIndirectObject();
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
static const char* scTwoByteRangeStart = "00";
static const char* scTwoByteRangeEnd = "FF";
static const char* scEndCodeSpaceRange = "endcodespacerange\n";
static const string scBeginBFChar = "beginbfchar";
static const string scEndBFChar = "endbfchar";
static const char* scCmapFooter = "endcmap CMapName currentdict /CMap defineresource pop end end\n";


void CFFANSIFontWriter::WriteToUnicodeMap(ObjectIDType inToUnicodeMap)
{
	mObjectsContext->StartNewIndirectObject(inToUnicodeMap);
	PDFStream* pdfStream = mObjectsContext->StartPDFStream();
	IByteWriter* cmapWriteContext = pdfStream->GetWriteStream();
	PrimitiveObjectsWriter primitiveWriter(cmapWriteContext);
	unsigned long i = 1;
	UIntAndGlyphEncodingInfoVector::iterator it = mCharactersVector.begin() + 1; // skip 0 glyph
	char formattingBuffer[13];

	cmapWriteContext->Write((const Byte*)scCmapHeader,strlen(scCmapHeader));
	primitiveWriter.WriteHexString(scTwoByteRangeStart);
	primitiveWriter.WriteHexString(scTwoByteRangeEnd,eTokenSeparatorEndLine);
	cmapWriteContext->Write((const Byte*)scEndCodeSpaceRange,strlen(scEndCodeSpaceRange));

	if(mCharactersVector.size() < 100)
		primitiveWriter.WriteInteger(mCharactersVector.size());
	else
		primitiveWriter.WriteInteger(100);
	primitiveWriter.WriteKeyword(scBeginBFChar);
	SAFE_SPRINTF_2(formattingBuffer,13,"<%02x> <%04x>\n",it->second.mEncodedCharacter,it->second.mUnicodeCharacter);
	cmapWriteContext->Write((const Byte*)formattingBuffer,12);
	++it;
	for(; it != mCharactersVector.end(); ++it,++i)
	{
		if(i % 100 == 0)
		{
			primitiveWriter.WriteKeyword(scEndBFChar);
			if(mCharactersVector.size() - i < 100)
				primitiveWriter.WriteInteger(mCharactersVector.size() - i);
			else
				primitiveWriter.WriteInteger(100);
			primitiveWriter.WriteKeyword(scBeginBFChar);
		}
		SAFE_SPRINTF_2(formattingBuffer,13,"<%02x> <%04x>\n",it->second.mEncodedCharacter,it->second.mUnicodeCharacter);
		cmapWriteContext->Write((const Byte*)formattingBuffer,12);
	}
	primitiveWriter.WriteKeyword(scEndBFChar);
	cmapWriteContext->Write((const Byte*)scCmapFooter,strlen(scCmapFooter));
	mObjectsContext->EndPDFStream(pdfStream);
}

static const string scFontName = "FontName";
static const string scFontFamily = "FontFamily";
static const string scFontStretch = "FontStretch";
static const string scFontWeight = "FontWeight";
static const string scFlags = "Flags";
static const string scFontBBox = "FontBBox";
static const string scItalicAngle = "ItalicAngle";
static const string scAscent = "Ascent";
static const string scDescent = "Descent";
static const string scCapHeight = "CapHeight";
static const string scXHeight = "XHeight";
static const string scStemV = "StemV";
static const string scFontFile3 = "FontFile3";
static const string scCharSet = "CharSet";

void CFFANSIFontWriter::WriteFontDescriptor(ObjectIDType inFontDescriptorObjectID,const string& inFontPostscriptName)
{
	DictionaryContext* fontDescriptorDictionary;

	mObjectsContext->StartNewIndirectObject(inFontDescriptorObjectID);
	fontDescriptorDictionary = mObjectsContext->StartDictionary();
	
	// FontName
	fontDescriptorDictionary->WriteKey(scFontName);
	fontDescriptorDictionary->WriteNameValue(inFontPostscriptName);

	// FontFamily
	fontDescriptorDictionary->WriteKey(scFontFamily);
	fontDescriptorDictionary->WriteLiteralStringValue((*mFontInfo)->family_name);

	// FontStretch
	fontDescriptorDictionary->WriteKey(scFontStretch);
	fontDescriptorDictionary->WriteNameValue(scFontStretchLabels[mFontInfo->GetFontStretch()]);

	// FontWeight
	fontDescriptorDictionary->WriteKey(scFontWeight);
	fontDescriptorDictionary->WriteIntegerValue(mFontInfo->GetFontWeight());

	// FontBBox
	fontDescriptorDictionary->WriteKey(scFontBBox);
	fontDescriptorDictionary->WriteRectangleValue(
											PDFRectangle(
												(*mFontInfo)->bbox.xMin,
												(*mFontInfo)->bbox.yMin,
												(*mFontInfo)->bbox.xMax,
												(*mFontInfo)->bbox.yMax));

	// ItalicAngle
	fontDescriptorDictionary->WriteKey(scItalicAngle);
	fontDescriptorDictionary->WriteDoubleValue(mFontInfo->GetItalicAngle());

	// Ascent
	fontDescriptorDictionary->WriteKey(scAscent);
	fontDescriptorDictionary->WriteIntegerValue((*mFontInfo)->ascender);

	// Descent
	fontDescriptorDictionary->WriteKey(scDescent);
	fontDescriptorDictionary->WriteIntegerValue((*mFontInfo)->descender);

	// CapHeight
	BoolAndFTShort result =  mFontInfo->GetCapHeight();
	if(result.first)
	{
		fontDescriptorDictionary->WriteKey(scCapHeight);
		fontDescriptorDictionary->WriteIntegerValue(result.second);
	}

	// XHeight
	result = mFontInfo->GetxHeight();
	if(result.first)
	{
		fontDescriptorDictionary->WriteKey(scXHeight);
		fontDescriptorDictionary->WriteIntegerValue(result.second);
	}


	// StemV
	fontDescriptorDictionary->WriteKey(scStemV);
	fontDescriptorDictionary->WriteIntegerValue(mFontInfo->GetStemV());

	// ChartSet
	fontDescriptorDictionary->WriteKey(scCharSet);
	WriteCharSet();

	// Flags
	fontDescriptorDictionary->WriteKey(scFlags);
	fontDescriptorDictionary->WriteIntegerValue(CalculateFlags());

	// FontFile3
	// TODO :) font embedding

	mObjectsContext->EndDictionary(fontDescriptorDictionary);
	mObjectsContext->EndIndirectObject();
}

static const Byte scLeftParanthesis[] = {'('};
static const Byte scRightParanthesis[] = {')'};
void CFFANSIFontWriter::WriteCharSet()
{
	// charset is a rather long string. i'd rather write it down as it goes, than format a string
	// and submit it.
	IByteWriterWithPosition* directStream = mObjectsContext->StartFreeContext();
	PrimitiveObjectsWriter primitiveWriter(directStream);

	directStream->Write(scLeftParanthesis,1);

	UIntAndGlyphEncodingInfoVector::iterator it = mCharactersVector.begin()+1; // skip 0 character
	char buffer[100];
	
	for(; it != mCharactersVector.end(); ++it)
	{
		FT_Get_Glyph_Name(*mFontInfo,it->first,buffer,100);
		primitiveWriter.WriteName(buffer,eTokenSepratorNone);
	}
	directStream->Write(scRightParanthesis,1);
	mObjectsContext->EndFreeContext();
	mObjectsContext->EndLine();
}

unsigned int CFFANSIFontWriter::CalculateFlags()
{
	unsigned int flags = 0;

	// see FreeTypeFaceWrapper::GetFontFlags for explanation, if you must

	if(mFontInfo->IsFixedPitch())
		flags |= 1;
	if(mFontInfo->IsSerif())
		flags |= 2;
	if(IsSymbolic())
		flags |= 4;
	else
		flags |= 32;
	if(mFontInfo->IsScript())
		flags |= 8;
	if(mFontInfo->IsItalic())
		flags |= 64;
	if(mFontInfo->IsForceBold())
		flags |= (1<<18);

	return flags;
	
}

bool CFFANSIFontWriter::IsSymbolic()
{
	return IsDefiningCharsNotInAdobeStandardLatin();
}

bool CFFANSIFontWriter::IsDefiningCharsNotInAdobeStandardLatin()
{
	bool hasOnlyAdobeStandard = true;

	UIntAndGlyphEncodingInfoVector::iterator it = mCharactersVector.begin()+1; // skip 0 character
	
	for(; it != mCharactersVector.end() && hasOnlyAdobeStandard; ++it)
		hasOnlyAdobeStandard = mFontInfo->IsCharachterCodeAdobeStandard(it->second.mUnicodeCharacter);
	return !hasOnlyAdobeStandard;
}

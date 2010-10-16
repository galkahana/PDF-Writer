#include "FontDescriptorWriter.h"
#include "FreeTypeFaceWrapper.h"
#include "ObjectsContext.h"
#include "EFontStretch.h"
#include "IFontDescriptorCharsetWriter.h"
#include "DictionaryContext.h"

FontDescriptorWriter::FontDescriptorWriter(void)
{
}

FontDescriptorWriter::~FontDescriptorWriter(void)
{
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

void FontDescriptorWriter::WriteFontDescriptor(	ObjectIDType inFontDescriptorObjectID,
												const string& inFontPostscriptName,
												FreeTypeFaceWrapper* inFontInfo,
												const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
												ObjectsContext* inObjectsContext,
												IFontDescriptorCharsetWriter* inCharsetWriter)
{
	DictionaryContext* fontDescriptorDictionary;

	inObjectsContext->StartNewIndirectObject(inFontDescriptorObjectID);
	fontDescriptorDictionary = inObjectsContext->StartDictionary();
	
	// FontName
	fontDescriptorDictionary->WriteKey(scFontName);
	fontDescriptorDictionary->WriteNameValue(inFontPostscriptName);

	// FontFamily
	fontDescriptorDictionary->WriteKey(scFontFamily);
	fontDescriptorDictionary->WriteLiteralStringValue((*inFontInfo)->family_name);

	// FontStretch
	fontDescriptorDictionary->WriteKey(scFontStretch);
	fontDescriptorDictionary->WriteNameValue(scFontStretchLabels[inFontInfo->GetFontStretch()]);

	// FontWeight
	fontDescriptorDictionary->WriteKey(scFontWeight);
	fontDescriptorDictionary->WriteIntegerValue(inFontInfo->GetFontWeight());

	// FontBBox
	fontDescriptorDictionary->WriteKey(scFontBBox);
	fontDescriptorDictionary->WriteRectangleValue(
											PDFRectangle(
												(*inFontInfo)->bbox.xMin,
												(*inFontInfo)->bbox.yMin,
												(*inFontInfo)->bbox.xMax,
												(*inFontInfo)->bbox.yMax));

	// ItalicAngle
	fontDescriptorDictionary->WriteKey(scItalicAngle);
	fontDescriptorDictionary->WriteDoubleValue(inFontInfo->GetItalicAngle());

	// Ascent
	fontDescriptorDictionary->WriteKey(scAscent);
	fontDescriptorDictionary->WriteIntegerValue((*inFontInfo)->ascender);

	// Descent
	fontDescriptorDictionary->WriteKey(scDescent);
	fontDescriptorDictionary->WriteIntegerValue((*inFontInfo)->descender);

	// CapHeight
	BoolAndFTShort result =  inFontInfo->GetCapHeight();
	if(result.first)
	{
		fontDescriptorDictionary->WriteKey(scCapHeight);
		fontDescriptorDictionary->WriteIntegerValue(result.second);
	}

	// XHeight
	result = inFontInfo->GetxHeight();
	if(result.first)
	{
		fontDescriptorDictionary->WriteKey(scXHeight);
		fontDescriptorDictionary->WriteIntegerValue(result.second);
	}


	// StemV
	fontDescriptorDictionary->WriteKey(scStemV);
	fontDescriptorDictionary->WriteIntegerValue(inFontInfo->GetStemV());

	// ChartSet writing (variants according to ANSI/CID)
	inCharsetWriter->WriteCharSet(fontDescriptorDictionary,inObjectsContext,inFontInfo,inEncodedGlyphs);

	// Flags
	fontDescriptorDictionary->WriteKey(scFlags);
	fontDescriptorDictionary->WriteIntegerValue(CalculateFlags(inFontInfo,inEncodedGlyphs));

	// FontFile3
	// TODO :) font embedding

	inObjectsContext->EndDictionary(fontDescriptorDictionary);
	inObjectsContext->EndIndirectObject();
}

unsigned int FontDescriptorWriter::CalculateFlags(	FreeTypeFaceWrapper* inFontInfo,
													const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs)
{
	unsigned int flags = 0;

	// see FreeTypeFaceWrapper::GetFontFlags for explanation, if you must

	if(inFontInfo->IsFixedPitch())
		flags |= 1;
	if(inFontInfo->IsSerif())
		flags |= 2;
	if(IsSymbolic(inFontInfo,inEncodedGlyphs))
		flags |= 4;
	else
		flags |= 32;
	if(inFontInfo->IsScript())
		flags |= 8;
	if(inFontInfo->IsItalic())
		flags |= 64;
	if(inFontInfo->IsForceBold())
		flags |= (1<<18);

	return flags;
	
}

bool FontDescriptorWriter::IsSymbolic(	FreeTypeFaceWrapper* inFontInfo,
										const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs)
{
	bool hasOnlyAdobeStandard = true;

	UIntAndGlyphEncodingInfoVector::const_iterator it = inEncodedGlyphs.begin()+1; // skip 0 character
	
	for(; it != inEncodedGlyphs.end() && hasOnlyAdobeStandard; ++it)
		hasOnlyAdobeStandard = inFontInfo->IsCharachterCodeAdobeStandard(it->second.mUnicodeCharacter);
	return !hasOnlyAdobeStandard;
}

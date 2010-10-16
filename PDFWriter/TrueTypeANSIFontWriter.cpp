#include "TrueTypeANSIFontWriter.h"
#include "ANSIFontWriter.h"
#include "DictionaryContext.h"

TrueTypeANSIFontWriter::TrueTypeANSIFontWriter(void)
{
}

TrueTypeANSIFontWriter::~TrueTypeANSIFontWriter(void)
{
}

EStatusCode TrueTypeANSIFontWriter::WriteFont(	FreeTypeFaceWrapper& inFontInfo,
											WrittenFontRepresentation* inFontOccurrence,
											ObjectsContext* inObjectsContext)
{
	ANSIFontWriter fontWriter;

	return fontWriter.WriteFont(inFontInfo,inFontOccurrence,inObjectsContext,this);
}

static const string scTrueType = "TrueType";

void TrueTypeANSIFontWriter::WriteSubTypeValue(DictionaryContext* inDictionary)
{
	inDictionary->WriteNameValue(scTrueType);
}

IFontDescriptorCharsetWriter* TrueTypeANSIFontWriter::GetCharsetWriter()
{
	// note that there's no charset writing for true types
	return this;
}

void TrueTypeANSIFontWriter::WriteCharSet(	DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext,
										FreeTypeFaceWrapper* inFontInfo,
										const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs)
{
	// do nothing. no charset writing for true types
}
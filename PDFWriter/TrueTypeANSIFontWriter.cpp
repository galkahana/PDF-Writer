#include "TrueTypeANSIFontWriter.h"
#include "ANSIFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "IndirectObjectsReferenceRegistry.h"
#include "TrueTypeEmbeddedFontWriter.h"
#include "FreeTypeFaceWrapper.h"
#include "Trace.h"

#include <ft2build.h>
#include FT_FREETYPE_H

TrueTypeANSIFontWriter::TrueTypeANSIFontWriter(void)
{
}

TrueTypeANSIFontWriter::~TrueTypeANSIFontWriter(void)
{
}

static const string scPlus = "+";
EStatusCode TrueTypeANSIFontWriter::WriteFont(	FreeTypeFaceWrapper& inFontInfo,
											WrittenFontRepresentation* inFontOccurrence,
											ObjectsContext* inObjectsContext)
{
	const char* postscriptFontName = FT_Get_Postscript_Name(inFontInfo);
	if(!postscriptFontName)
	{
		TRACE_LOG("TrueTypeANSIFontWriter::WriteFont, unexpected failure. no postscript font name for font");
		return eFailure;
	}
	std::string subsetFontName = inObjectsContext->GenerateSubsetFontPrefix() + scPlus + postscriptFontName;

	// reset embedded font object ID (and flag...to whether it was actually embedded or not, which may 
	// happen due to font embedding restrictions)
	mEmbeddedFontFileObjectID = 0;

	TrueTypeEmbeddedFontWriter embeddedFontWriter;

	EStatusCode status = embeddedFontWriter.WriteEmbeddedFont(	inFontInfo,
																inFontOccurrence->GetGlyphIDsAsOrderedVector(),
																inObjectsContext,
																mEmbeddedFontFileObjectID);
	if(eFailure == status)
		return status;

	ANSIFontWriter fontWriter;

	return fontWriter.WriteFont(inFontInfo,inFontOccurrence,inObjectsContext,this,subsetFontName);
}

static const string scTrueType = "TrueType";

void TrueTypeANSIFontWriter::WriteSubTypeValue(DictionaryContext* inDictionary)
{
	inDictionary->WriteNameValue(scTrueType);
}

IFontDescriptorHelper* TrueTypeANSIFontWriter::GetCharsetWriter()
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

static const string scFontFile2 = "FontFile2";

void TrueTypeANSIFontWriter::WriteFontFileReference(	
										DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext)
{
	// will be 0 in case embedding didn't occur due to font embedding restrictions
	if(mEmbeddedFontFileObjectID != 0)
	{
		// FontFile2
		inDescriptorContext->WriteKey(scFontFile2);
		inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
	}
}

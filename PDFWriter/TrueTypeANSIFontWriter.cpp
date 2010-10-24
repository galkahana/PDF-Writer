#include "TrueTypeANSIFontWriter.h"
#include "ANSIFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "IndirectObjectsReferenceRegistry.h"
#include "TrueTypeEmbeddedFontWriter.h"

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

	EStatusCode status = fontWriter.WriteFont(inFontInfo,inFontOccurrence,inObjectsContext,this);

	if(eFailure == status)
		return status;

	TrueTypeEmbeddedFontWriter embeddedFontWriter;

	return embeddedFontWriter.WriteEmbeddedFont(inFontInfo,inFontOccurrence->GetGlyphIDsAsOrderedVector(),mEmbeddedFontFileObjectID,inObjectsContext);
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
	// FontFile2
	inDescriptorContext->WriteNameValue(scFontFile2);
	mEmbeddedFontFileObjectID = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
}

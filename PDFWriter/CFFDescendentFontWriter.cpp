#include "CFFDescendentFontWriter.h"
#include "DescendentFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"

CFFDescendentFontWriter::CFFDescendentFontWriter(void)
{
}

CFFDescendentFontWriter::~CFFDescendentFontWriter(void)
{
}

EStatusCode CFFDescendentFontWriter::WriteFont(	ObjectIDType inDecendentObjectID, 
														const string& inFontName,
														FreeTypeFaceWrapper& inFontInfo,
														const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
														ObjectsContext* inObjectsContext)
{
	DescendentFontWriter descendentFontWriter;

	return descendentFontWriter.WriteFont(inDecendentObjectID,inFontName,inFontInfo,inEncodedGlyphs,inObjectsContext,this);

	// TODO : write font program
}

static const string scCIDFontType0 = "CIDFontType0";

void CFFDescendentFontWriter::WriteSubTypeValue(DictionaryContext* inDescendentFontContext)
{
	inDescendentFontContext->WriteNameValue(scCIDFontType0);
}

void CFFDescendentFontWriter::WriteAdditionalKeys(DictionaryContext* inDescendentFontContext)
{
	// do nothing
}

static const string scFontFile3 = "FontFile3";
void CFFDescendentFontWriter::WriteFontFileReference(DictionaryContext* inDescriptorContext,
													ObjectsContext* inObjectsContext)
{
	// FontFile3
	inDescriptorContext->WriteNameValue(scFontFile3);
	mEmbeddedFontFileObjectID = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);	
}

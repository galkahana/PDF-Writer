#include "TrueTypeDescendentFontWriter.h"
#include "DescendentFontWriter.h"
#include "DictionaryContext.h"
#include "TrueTypeEmbeddedFontWriter.h"
#include "ObjectsContext.h"
#include "IndirectObjectsReferenceRegistry.h"

TrueTypeDescendentFontWriter::TrueTypeDescendentFontWriter(void)
{
}

TrueTypeDescendentFontWriter::~TrueTypeDescendentFontWriter(void)
{
}

static UIntVector GetOrderedKeys(const UIntAndGlyphEncodingInfoVector& inMap)
{
	UIntVector result;
	for(UIntAndGlyphEncodingInfoVector::const_iterator it = inMap.begin(); it != inMap.end(); ++it)
		result.push_back(it->first);
	sort(result.begin(),result.end());
	return result;
}


EStatusCode TrueTypeDescendentFontWriter::WriteFont(	ObjectIDType inDecendentObjectID, 
														const string& inFontName,
														FreeTypeFaceWrapper& inFontInfo,
														const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
														ObjectsContext* inObjectsContext)
{
	DescendentFontWriter descendentFontWriter;

	EStatusCode status = descendentFontWriter.WriteFont(inDecendentObjectID,inFontName,inFontInfo,inEncodedGlyphs,inObjectsContext,this);

	if(eFailure == status)
		return status;

	TrueTypeEmbeddedFontWriter embeddedFontWriter;

	return embeddedFontWriter.WriteEmbeddedFont(inFontInfo,GetOrderedKeys(inEncodedGlyphs),mEmbeddedFontFileObjectID,inObjectsContext);
}

static const string scCIDFontType2 = "CIDFontType2";

void TrueTypeDescendentFontWriter::WriteSubTypeValue(DictionaryContext* inDescendentFontContext)
{
	inDescendentFontContext->WriteNameValue(scCIDFontType2);
}

static const string scCIDToGIDMap = "CIDToGIDMap";
static const string scIdentity = "Identity";

void TrueTypeDescendentFontWriter::WriteAdditionalKeys(DictionaryContext* inDescendentFontContext)
{
	// CIDToGIDMap 
	inDescendentFontContext->WriteKey(scCIDToGIDMap);
	inDescendentFontContext->WriteNameValue(scIdentity);
}

static const string scFontFile2 = "FontFile2";
void TrueTypeDescendentFontWriter::WriteFontFileReference(	
										DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext)
{
	// FontFile2
	inDescriptorContext->WriteNameValue(scFontFile2);
	mEmbeddedFontFileObjectID = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
}
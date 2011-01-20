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
	// reset embedded font object ID (and flag...to whether it was actually embedded or not, which may 
	// happen due to font embedding restrictions)
	mEmbeddedFontFileObjectID = 0;

	TrueTypeEmbeddedFontWriter embeddedFontWriter;
	EStatusCode status = embeddedFontWriter.WriteEmbeddedFont(inFontInfo,GetOrderedKeys(inEncodedGlyphs),inObjectsContext,mEmbeddedFontFileObjectID);

	if(eFailure == status)
		return status;

	DescendentFontWriter descendentFontWriter;
	return descendentFontWriter.WriteFont(inDecendentObjectID,inFontName,inFontInfo,inEncodedGlyphs,inObjectsContext,this);
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
	// write font reference only if there's what to write....
	if(mEmbeddedFontFileObjectID != 0)
	{
		// FontFile2
		inDescriptorContext->WriteKey(scFontFile2);
		inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
	}
}
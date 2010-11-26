#include "CFFDescendentFontWriter.h"
#include "DescendentFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "CFFEmbeddedFontWriter.h"

CFFDescendentFontWriter::CFFDescendentFontWriter(void)
{
}

CFFDescendentFontWriter::~CFFDescendentFontWriter(void)
{
}

static bool sEncodedGlypsSort(const UIntAndGlyphEncodingInfo& inLeft, const UIntAndGlyphEncodingInfo& inRight)
{
	return inLeft.first < inRight.first;
}

static const string scCIDFontType0C = "CIDFontType0C";
EStatusCode CFFDescendentFontWriter::WriteFont(	ObjectIDType inDecendentObjectID, 
														const string& inFontName,
														FreeTypeFaceWrapper& inFontInfo,
														const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
														ObjectsContext* inObjectsContext)
{
	DescendentFontWriter descendentFontWriter;
	string subsetFontName;

	EStatusCode status = descendentFontWriter.WriteFont(inDecendentObjectID,inFontName,inFontInfo,inEncodedGlyphs,inObjectsContext,this,subsetFontName);

	if(eFailure == status)
		return status;

	CFFEmbeddedFontWriter embeddedFontWriter;
	UIntAndGlyphEncodingInfoVector encodedGlyphs = inEncodedGlyphs;
	UIntVector orderedGlyphs;
	UShortVector cidMapping;
	
	sort(encodedGlyphs.begin(),encodedGlyphs.end(),sEncodedGlypsSort);	

	for(UIntAndGlyphEncodingInfoVector::const_iterator it = encodedGlyphs.begin(); 
		it != encodedGlyphs.end(); 
		++it)
	{
		orderedGlyphs.push_back(it->first);
		cidMapping.push_back(it->second.mEncodedCharacter);
	}
	return embeddedFontWriter.WriteEmbeddedFont(inFontInfo,
												orderedGlyphs,
												scCIDFontType0C,
												mEmbeddedFontFileObjectID,
												subsetFontName,
												inObjectsContext,
												&cidMapping);
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
	inDescriptorContext->WriteKey(scFontFile3);
	mEmbeddedFontFileObjectID = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);	
}

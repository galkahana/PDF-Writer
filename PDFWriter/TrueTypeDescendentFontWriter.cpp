#include "TrueTypeDescendentFontWriter.h"
#include "DescendentFontWriter.h"
#include "DictionaryContext.h"

TrueTypeDescendentFontWriter::TrueTypeDescendentFontWriter(void)
{
}

TrueTypeDescendentFontWriter::~TrueTypeDescendentFontWriter(void)
{
}

EStatusCode TrueTypeDescendentFontWriter::WriteFont(	ObjectIDType inDecendentObjectID, 
														const string& inFontName,
														FreeTypeFaceWrapper& inFontInfo,
														const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
														ObjectsContext* inObjectsContext)
{
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
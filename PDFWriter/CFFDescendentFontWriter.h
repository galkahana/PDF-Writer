#pragma once
#include "IDescendentFontWriter.h"

class CFFDescendentFontWriter: public IDescendentFontWriter
{
public:
	CFFDescendentFontWriter(void);
	~CFFDescendentFontWriter(void);

	// IDescendentFontWriter implementation [used also as helper for the DescendentFontWriter]
	virtual EStatusCode WriteFont(	ObjectIDType inDecendentObjectID, 
									const string& inFontName,
									FreeTypeFaceWrapper& inFontInfo,
									const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
									ObjectsContext* inObjectsContext);
	virtual void WriteSubTypeValue(DictionaryContext* inDescendentFontContext);
	virtual void WriteAdditionalKeys(DictionaryContext* inDescendentFontContext);
	virtual void WriteFontFileReference(DictionaryContext* inDescriptorContext,
									ObjectsContext* inObjectsContext);

private:
	ObjectIDType mEmbeddedFontFileObjectID;

};
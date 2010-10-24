#pragma once

#include "EStatusCode.h"
#include "IANSIFontWriterHelper.h"
#include "IFontDescriptorHelper.h"
#include "ObjectsBasicTypes.h"

class FreeTypeFaceWrapper;
struct WrittenFontRepresentation;
class ObjectsContext;

class TrueTypeANSIFontWriter : public IANSIFontWriterHelper, IFontDescriptorHelper
{
public:
	TrueTypeANSIFontWriter(void);
	~TrueTypeANSIFontWriter(void);

	EStatusCode WriteFont(	FreeTypeFaceWrapper& inFontInfo,
							WrittenFontRepresentation* inFontOccurrence,
							ObjectsContext* inObjectsContext);

	// IANSIFontWriterHelper implementation
	virtual void WriteSubTypeValue(DictionaryContext* inDictionary);
	virtual IFontDescriptorHelper* GetCharsetWriter();

	// IFontDescriptorHelper implementation
	virtual void WriteCharSet(	DictionaryContext* inDescriptorContext,
								ObjectsContext* inObjectsContext,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
	virtual void WriteFontFileReference(DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext);

private:

	ObjectIDType mEmbeddedFontFileObjectID;
};

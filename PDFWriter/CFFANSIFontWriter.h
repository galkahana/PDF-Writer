#pragma once

#include "EStatusCode.h"
#include "IANSIFontWriterHelper.h"
#include "IFontDescriptorHelper.h"

class FreeTypeFaceWrapper;
struct WrittenFontRepresentation;
class ObjectsContext;


class CFFANSIFontWriter : public IANSIFontWriterHelper, IFontDescriptorHelper
{
public:
	CFFANSIFontWriter(void);
	~CFFANSIFontWriter(void);


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

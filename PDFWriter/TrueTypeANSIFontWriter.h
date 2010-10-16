#pragma once

#include "EStatusCode.h"
#include "IANSIFontWriterHelper.h"
#include "IFontDescriptorCharsetWriter.h"

class FreeTypeFaceWrapper;
struct WrittenFontRepresentation;
class ObjectsContext;

class TrueTypeANSIFontWriter : public IANSIFontWriterHelper, IFontDescriptorCharsetWriter
{
public:
	TrueTypeANSIFontWriter(void);
	~TrueTypeANSIFontWriter(void);

	EStatusCode WriteFont(	FreeTypeFaceWrapper& inFontInfo,
							WrittenFontRepresentation* inFontOccurrence,
							ObjectsContext* inObjectsContext);

	// IANSIFontWriterHelper implementation
	virtual void WriteSubTypeValue(DictionaryContext* inDictionary);
	virtual IFontDescriptorCharsetWriter* GetCharsetWriter();

	// IFontDescriptorCharsetWriter implementation
	virtual void WriteCharSet(	DictionaryContext* inDescriptorContext,
								ObjectsContext* inObjectsContext,
								FreeTypeFaceWrapper* inFontInfo,
								const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs);
};

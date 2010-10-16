#pragma once

#include "EStatusCode.h"
#include "WrittenFontRepresentation.h"
#include "ObjectsBasicTypes.h"

#include <utility>
#include <vector>

class FreeTypeFaceWrapper;
class ObjectsContext;
class DictionaryContext;
class IDescendentFontWriter;

using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class CIDFontWriter
{
public:
	CIDFontWriter(void);
	virtual ~CIDFontWriter(void);

	EStatusCode WriteFont(	FreeTypeFaceWrapper& inFontInfo,
							WrittenFontRepresentation* inFontOccurrence,
							ObjectsContext* inObjectsContext,
							IDescendentFontWriter* inDescendentFontWriter);

private:

	FreeTypeFaceWrapper* mFontInfo;
	WrittenFontRepresentation* mFontOccurrence;
	ObjectsContext* mObjectsContext;
	UIntAndGlyphEncodingInfoVector mCharactersVector;


	void WriteEncoding(DictionaryContext* inFontContext);
	void CalculateCharacterEncodingArray();
	void WriteToUnicodeMap(ObjectIDType inToUnicodeMap);
};

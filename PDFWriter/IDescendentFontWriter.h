#pragma once
#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <string>
#include <vector>
#include <utility>

class FreeTypeFaceWrapper;
class ObjectsContext;
class DictionaryContext;

using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;

class IDescendentFontWriter
{
public:
	virtual ~IDescendentFontWriter(){};

	virtual EStatusCode WriteFont(	ObjectIDType inDecendentObjectID, 
									const string& inFontName,
									FreeTypeFaceWrapper& inFontInfo,
									const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs,
									ObjectsContext* inObjectsContext) = 0;

	virtual void WriteSubTypeValue(DictionaryContext* inDescendentFontContext) = 0;

	// free writing of additional keys
	virtual void WriteAdditionalKeys(DictionaryContext* inDescendentFontContext) = 0;

};
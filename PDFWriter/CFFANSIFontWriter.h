#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "WrittenFontRepresentation.h"

#include <utility>
#include <vector>
#include <string>
#include <list>

class FreeTypeFaceWrapper;
class ObjectsContext;
class DictionaryContext;

using namespace std;

typedef pair<unsigned int, GlyphEncodingInfo> UIntAndGlyphEncodingInfo;
typedef vector<UIntAndGlyphEncodingInfo> UIntAndGlyphEncodingInfoVector;
typedef pair<unsigned short,string>  UShortAndString;
typedef list<UShortAndString> UShortAndStringList;

class CFFANSIFontWriter
{
public:
	CFFANSIFontWriter(void);
	~CFFANSIFontWriter(void);


	EStatusCode WriteFont(	FreeTypeFaceWrapper& inFontInfo,
							WrittenFontRepresentation* inFontOccurrence,
							ObjectsContext* inObjectsContext);

private:

	void CalculateCharacterEncodingArray();
	void WriteWidths(DictionaryContext* inFontContext);
	void CalculateDifferences();
	void WriteEncoding(DictionaryContext* inFontContext);
	void WriteEncodingDictionary();
	void WriteToUnicodeMap(ObjectIDType inToUnicodeMap);
	void WriteFontDescriptor(ObjectIDType inFontDescriptorObjectID,const string& inFontPostscriptName);
	void WriteCharSet();
	unsigned int CalculateFlags();
	bool IsSymbolic();
	bool IsDefiningCharsNotInAdobeStandardLatin();

	FreeTypeFaceWrapper* mFontInfo;
	WrittenFontRepresentation* mFontOccurrence;
	ObjectsContext* mObjectsContext;

	UIntAndGlyphEncodingInfoVector mCharactersVector;
	UShortAndStringList mDifferences;
	ObjectIDType mEncodingDictionaryID;
};

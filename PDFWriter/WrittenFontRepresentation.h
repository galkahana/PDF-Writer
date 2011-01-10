#pragma once

#include "ObjectsBasicTypes.h"

#include <map>
#include <algorithm>
#include <vector>

using namespace std;



struct GlyphEncodingInfo
{
	unsigned short mEncodedCharacter;
	unsigned long mUnicodeCharacter;

	GlyphEncodingInfo(unsigned short inEncodedCharacter,unsigned long inUnicodeCharacter)
	{mEncodedCharacter = inEncodedCharacter; mUnicodeCharacter = inUnicodeCharacter;}
};

typedef map<unsigned int, GlyphEncodingInfo> UIntToGlyphEncodingInfoMap;
typedef vector<unsigned int> UIntVector;

static UIntVector GetOrderedKeys(const UIntToGlyphEncodingInfoMap& inMap)
{
	UIntVector result;
	for(UIntToGlyphEncodingInfoMap::const_iterator it = inMap.begin(); it != inMap.end(); ++it)
		result.push_back(it->first);
	sort(result.begin(),result.end());
	return result;
}


struct WrittenFontRepresentation
{	
	WrittenFontRepresentation(){mWrittenObjectID = 0;}

	UIntToGlyphEncodingInfoMap mGlyphIDToEncodedChar;
	ObjectIDType mWrittenObjectID;

	UIntVector GetGlyphIDsAsOrderedVector() 
		{
			return GetOrderedKeys(mGlyphIDToEncodedChar);
		}
};

#pragma once
#include "AbstractWrittenFont.h"

#include <utility>
#include <list>
#include <map>

using namespace std;

typedef pair<unsigned char,unsigned char> UCharAndUChar;
typedef list<UCharAndUChar> UCharAndUCharList;
typedef map<unsigned int, unsigned char> UIntToUCharMap;


class WrittenFontCFF : public AbstractWrittenFont
{
public:
	WrittenFontCFF(ObjectsContext* inObjectsContext);
	virtual ~WrittenFontCFF(void);

private:
	virtual bool AddToANSIRepresentation(const wstring& inText,
							 const UIntList& inGlyphsList,
							 UShortList& outEncodedCharacters);


	bool HasEnoughSpaceForGlyphs(const UIntList& inGlyphsList);
	unsigned short EncodeGlyph(unsigned int inGlyph,wchar_t inCharacter);
	void RemoveFromFreeList(unsigned char inAllocatedPosition);
	unsigned char AllocateFromFreeList(unsigned int inGlyph);

	unsigned char mAvailablePositionsCount;
	UCharAndUCharList mFreeList;
	unsigned int mAssignedPositions[256];
	UIntToUCharMap mGlyphsInPresentation;

};

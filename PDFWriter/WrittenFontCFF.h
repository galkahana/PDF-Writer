#pragma once
#include "AbstractWrittenFont.h"

#include <utility>
#include <list>

using namespace std;

typedef pair<unsigned char,unsigned char> UCharAndUChar;
typedef list<UCharAndUChar> UCharAndUCharList;


class WrittenFontCFF : public AbstractWrittenFont
{
public:
	WrittenFontCFF(ObjectsContext* inObjectsContext,bool inIsCID);
	virtual ~WrittenFontCFF(void);


	virtual EStatusCode WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo);


private:
	virtual bool AddToANSIRepresentation(const wstring& inText,
							 const UIntList& inGlyphsList,
							 UShortList& outEncodedCharacters);

	virtual bool AddToANSIRepresentation(const WStringList& inText,
							const UIntListList& inGlyphsList,
							UShortListList& outEncodedCharacters);

	bool HasEnoughSpaceForGlyphs(const UIntList& inGlyphsList);
	unsigned short EncodeGlyph(unsigned int inGlyph,wchar_t inCharacter);
	void RemoveFromFreeList(unsigned char inAllocatedPosition);
	unsigned char AllocateFromFreeList(unsigned int inGlyph);
	bool HasEnoughSpaceForGlyphs(const UIntListList& inGlyphsList);

	unsigned char mAvailablePositionsCount;
	UCharAndUCharList mFreeList;
	unsigned int mAssignedPositions[256];
	bool mIsCID;
};

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
	virtual bool AddToANSIRepresentation(const GlyphUnicodeMappingList& inGlyphsList,
										 UShortList& outEncodedCharacters);

	virtual bool AddToANSIRepresentation(const GlyphUnicodeMappingListList& inGlyphsList,
										UShortListList& outEncodedCharacters);

	bool HasEnoughSpaceForGlyphs(const GlyphUnicodeMappingList& inGlyphsList);
	unsigned short EncodeGlyph(unsigned int inGlyph,const ULongVector& inCharacters);
	void RemoveFromFreeList(unsigned char inAllocatedPosition);
	unsigned char AllocateFromFreeList(unsigned int inGlyph);
	bool HasEnoughSpaceForGlyphs(const GlyphUnicodeMappingListList& inGlyphsList);

	unsigned char mAvailablePositionsCount;
	UCharAndUCharList mFreeList;
	unsigned int mAssignedPositions[256];
	bool mIsCID;
};

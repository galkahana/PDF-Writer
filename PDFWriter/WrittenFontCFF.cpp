#include "WrittenFontCFF.h"

WrittenFontCFF::WrittenFontCFF(ObjectsContext* inObjectsContext):AbstractWrittenFont(inObjectsContext)
{
	mAvailablePositionsCount = 255;
	mFreeList.push_back(UCharAndUChar(1,255)); 
	// 1st place is reserved for .notdef/0 glyph index. we'll use 0s in the array in all other places as indication for avialability
	for(int i=0;i<256;++i) 
		mAssignedPositions[i] = 0;
	mGlyphsInPresentation.insert(UIntToUCharMap::value_type(0,0));
}

WrittenFontCFF::~WrittenFontCFF(void)
{
}

bool WrittenFontCFF::AddToANSIRepresentation(
						const wstring& inText,
						const UIntList& inGlyphsList,
						UShortList& outEncodedCharacters)
{
	if(HasEnoughSpaceForGlyphs(inGlyphsList))
	{
		UIntList::const_iterator it = inGlyphsList.begin();
		wstring::const_iterator itText = inText.begin(); // assuming 1-1 match for now

		for(; it != inGlyphsList.end(); ++it,++itText)
			outEncodedCharacters.push_back(EncodeGlyph(*it,*itText));
		return true;
	}
	else
		return false;
}

bool WrittenFontCFF::HasEnoughSpaceForGlyphs(const UIntList& inGlyphsList)
{
	UIntList::const_iterator it = inGlyphsList.begin();
	int glyphsToAddCount = 0;

	for(; it != inGlyphsList.end(); ++it)
		if(mGlyphsInPresentation.find(*it) == mGlyphsInPresentation.end())
			++glyphsToAddCount;

	return glyphsToAddCount > mAvailablePositionsCount;
}

unsigned short WrittenFontCFF::EncodeGlyph(unsigned int inGlyph,wchar_t inCharacter)
{
	UIntToUCharMap::iterator it = mGlyphsInPresentation.find(inGlyph);

	if(it == mGlyphsInPresentation.end())
	{
		// as a default position, i'm grabbing the ansi bits. this should display nice charachters, when possible
		unsigned char encoding = (unsigned char)(inCharacter & 0xff);
		if(mAssignedPositions[encoding] == 0)
			RemoveFromFreeList(encoding);
		else
			encoding = AllocateFromFreeList(inGlyph);
		mAssignedPositions[encoding] = inGlyph;
		mGlyphsInPresentation.insert(UIntToUCharMap::value_type(inGlyph,encoding));			
		--mAvailablePositionsCount;
	}
	return it->second;
}

void WrittenFontCFF::RemoveFromFreeList(unsigned char inAllocatedPosition)
{
	// yeah yeah, you want binary search. i hear you.
	UCharAndUCharList::iterator it=mFreeList.begin();

	for(; it != mFreeList.end();++it)
	{
		if(it->first <= inAllocatedPosition && inAllocatedPosition <=it->second)
		{
			// found range! now remove the allocated position from it...got a couple of options
			if(it->first == it->second)
			{
				mFreeList.erase(it);
			}
			else if(it->first == inAllocatedPosition)
			{
				++it->first;
			}
			else if(it->second == inAllocatedPosition)
			{
				--it->second;
			}
			else
			{
				UCharAndUChar newPair(it->first,inAllocatedPosition-1);
				it->first = inAllocatedPosition+1;
				mFreeList.insert(it,newPair);
			}
			break;
		}
	}
}

unsigned char WrittenFontCFF::AllocateFromFreeList(unsigned int inGlyph)
{
	// just allocate the first available position
	UCharAndUCharList::iterator it=mFreeList.begin();
	unsigned char result = it->first;
	
	if(it->first == it->second)
		mFreeList.erase(it);
	else
		++(it->first);
	return result;
}
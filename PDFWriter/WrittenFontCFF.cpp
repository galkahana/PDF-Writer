#include "WrittenFontCFF.h"
#include "Trace.h"
#include "CFFANSIFontWriter.h"
#include "CIDFontWriter.h"
#include "CFFDescendentFontWriter.h"

WrittenFontCFF::WrittenFontCFF(ObjectsContext* inObjectsContext):AbstractWrittenFont(inObjectsContext)
{
	mAvailablePositionsCount = 255;
	mFreeList.push_back(UCharAndUChar(1,255)); 
	// 1st place is reserved for .notdef/0 glyph index. we'll use 0s in the array in all other places as indication for avialability
	for(int i=0;i<256;++i) 
		mAssignedPositions[i] = 0;
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
		if(mANSIRepresentation->mGlyphIDToEncodedChar.find(*it) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
			++glyphsToAddCount;

	return glyphsToAddCount <= mAvailablePositionsCount;
}

unsigned short WrittenFontCFF::EncodeGlyph(unsigned int inGlyph,wchar_t inCharacter)
{
	// for the first time, add also 0,0 mapping
	if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
		mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));

	UIntToGlyphEncodingInfoMap::iterator it = mANSIRepresentation->mGlyphIDToEncodedChar.find(inGlyph);

	if(it == mANSIRepresentation->mGlyphIDToEncodedChar.end())
	{
		// as a default position, i'm grabbing the ansi bits. this should display nice charachters, when possible
		unsigned char encoding = (unsigned char)(inCharacter & 0xff);
		if(mAssignedPositions[encoding] == 0)
			RemoveFromFreeList(encoding);
		else
			encoding = AllocateFromFreeList(inGlyph);
		mAssignedPositions[encoding] = inGlyph;
		it = mANSIRepresentation->mGlyphIDToEncodedChar.insert(
				UIntToGlyphEncodingInfoMap::value_type(inGlyph,GlyphEncodingInfo(encoding,inCharacter))).first;			
		--mAvailablePositionsCount;
	}
	return it->second.mEncodedCharacter;
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

EStatusCode WrittenFontCFF::WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo)
{
	EStatusCode status = eSuccess;
	do
	{
		if(mANSIRepresentation && mANSIRepresentation->mWrittenObjectID != 0)
		{
			CFFANSIFontWriter fontWriter;

			status = fontWriter.WriteFont(inFontInfo,mANSIRepresentation,mObjectsContext);
			if(status != eSuccess)
			{
				TRACE_LOG("WrittenFontCFF::WriteFontDefinition, Failed to write Ansi font definition");
				break;

			}
		}

		if(mCIDRepresentation && mCIDRepresentation->mWrittenObjectID != 0)
		{
			CIDFontWriter fontWriter;
			CFFDescendentFontWriter descendentFontWriter;

			status = fontWriter.WriteFont(inFontInfo,mCIDRepresentation,mObjectsContext,&descendentFontWriter);
			if(status != eSuccess)
			{
				TRACE_LOG("WrittenFontCFF::WriteFontDefinition, Failed to write CID font definition");
				break;
			}
		}

	} while(false);

	return status;
}

bool WrittenFontCFF::AddToANSIRepresentation(	const WStringList& inText,
												const UIntListList& inGlyphsList,
												UShortListList& outEncodedCharacters)
{
	if(HasEnoughSpaceForGlyphs(inGlyphsList))
	{
		UIntListList::const_iterator itList = inGlyphsList.begin();
		WStringList::const_iterator itTextList = inText.begin();
		UIntList::const_iterator it;
		wstring::const_iterator itText;
		UShortList encodedCharacters;

		for(; itList != inGlyphsList.end(); ++itList,++itTextList)
		{
			it = itList->begin();
			itText = itTextList->begin();
			for(; it != itList->end(); ++it,itText)
				encodedCharacters.push_back(EncodeGlyph(*it,*itText));
			outEncodedCharacters.push_back(encodedCharacters);
			encodedCharacters.clear();

		}
		return true;
	}
	else
		return false;
}

bool WrittenFontCFF::HasEnoughSpaceForGlyphs(const UIntListList& inGlyphsList)
{
	UIntListList::const_iterator itList = inGlyphsList.begin();
	UIntList::const_iterator it;
	int glyphsToAddCount = 0;

	for(; itList != inGlyphsList.end(); ++itList)
	{
		it = itList->begin();
		for(; it != itList->end(); ++it)
			if(mANSIRepresentation->mGlyphIDToEncodedChar.find(*it) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
				++glyphsToAddCount;
	}

	return glyphsToAddCount <= mAvailablePositionsCount;
}
#include "AbstractWrittenFont.h"
#include "ObjectsContext.h"
#include "InDirectObjectsReferenceRegistry.h"
#include "Trace.h"

AbstractWrittenFont::AbstractWrittenFont(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
	mCIDRepresentation = NULL;
	mANSIRepresentation = NULL;
}

AbstractWrittenFont::~AbstractWrittenFont(void)
{
}

void AbstractWrittenFont::AppendGlyphs(
						  const GlyphUnicodeMappingList& inGlyphsList,
						  UShortList& outEncodedCharacters,
						  bool& outEncodingIsMultiByte,
						  ObjectIDType &outFontObjectID)
{
	// so here the story goes:

	// if all strings glyphs exist in CID representation, use it. CID gets preference, being the one that should be
	// used at all times, once the first usage of it occured. if all included...no glyphs added, good.
	if(mCIDRepresentation && CanEncodeWithIncludedChars(mCIDRepresentation,inGlyphsList,outEncodedCharacters))
	{
		outFontObjectID = mCIDRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = true;
		return;
	}

	// k. no need to be hard...if by chance it's not in the CID (or CID does not exist yet) but is in the
	// ANSI representation - use it. no new glyphs added, everyone's happy
	if(mANSIRepresentation && CanEncodeWithIncludedChars(mANSIRepresentation,inGlyphsList,outEncodedCharacters))
	{
		outFontObjectID = mANSIRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = false;
		return;
	}

	// So it looks like we need to add glyphs.
	// if a CID representation exists - prefer it over the ANSI.
	if(mCIDRepresentation)
	{
		AddToCIDRepresentation(inGlyphsList,outEncodedCharacters);
		outFontObjectID = mCIDRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = true;
		return;
	}

	// if CID does not yet exist, try going for ANSI. it is, after all, more efficient.
	// but - consider that it might not be possible to encode the string
	if(!mANSIRepresentation)
		mANSIRepresentation = new WrittenFontRepresentation();

	// [note that each font type will have a different set of rules as to whether the glyphs
	// may be used in an ANSI representation]
	if(AddToANSIRepresentation(inGlyphsList,outEncodedCharacters))
	{
		if(0 == mANSIRepresentation->mWrittenObjectID)
			mANSIRepresentation->mWrittenObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

		outFontObjectID = mANSIRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = false;
		return;			
	}

	// if not...then create a CID representation and include the chars there. from now one...every time glyphs needs to be added
	// this algorithm will use the CID representation.
	mCIDRepresentation = new WrittenFontRepresentation();
	AddToCIDRepresentation(inGlyphsList,outEncodedCharacters);
	outFontObjectID = mCIDRepresentation->mWrittenObjectID;
	outEncodingIsMultiByte = true;
}

bool AbstractWrittenFont::CanEncodeWithIncludedChars(WrittenFontRepresentation* inRepresentation, 
													 const GlyphUnicodeMappingList& inGlyphsList,
													 UShortList& outEncodedCharacters)
{
	UShortList candidateEncoding;
	GlyphUnicodeMappingList::const_iterator it=inGlyphsList.begin();
	UIntToGlyphEncodingInfoMap::iterator itEncoding;
	bool allIncluded = true;

	for(; it != inGlyphsList.end() && allIncluded; ++it)
	{
		itEncoding = inRepresentation->mGlyphIDToEncodedChar.find(it->mGlyphCode);
		if(itEncoding == inRepresentation->mGlyphIDToEncodedChar.end())
			allIncluded = false;
		else
			candidateEncoding.push_back(itEncoding->second.mEncodedCharacter);
	}

	if(allIncluded)
		outEncodedCharacters = candidateEncoding;
	return allIncluded;
}

void AbstractWrittenFont::AddToCIDRepresentation(const GlyphUnicodeMappingList& inGlyphsList,
												 UShortList& outEncodedCharacters)
{
	// Glyph IDs are always used as CIDs, there's a possible @#$@up here if the font will contain too many glyphs...oops.
	// take care of this sometimes.

	// for the first time, add also 0,0 mapping
	if(mCIDRepresentation->mGlyphIDToEncodedChar.size() == 0)
		mCIDRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


	GlyphUnicodeMappingList::const_iterator it=inGlyphsList.begin();
	UIntToGlyphEncodingInfoMap::iterator itEncoding;

	for(; it != inGlyphsList.end(); ++it)
	{
		itEncoding = mCIDRepresentation->mGlyphIDToEncodedChar.find(it->mGlyphCode);
		if(itEncoding == mCIDRepresentation->mGlyphIDToEncodedChar.end())
		{
			itEncoding = mCIDRepresentation->mGlyphIDToEncodedChar.insert(
				UIntToGlyphEncodingInfoMap::value_type(it->mGlyphCode,GlyphEncodingInfo(it->mGlyphCode,it->mUnicodeValues))).first;

		}
		outEncodedCharacters.push_back(itEncoding->second.mEncodedCharacter);
	}

	if(0 == mCIDRepresentation->mWrittenObjectID)
		mCIDRepresentation->mWrittenObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
}

/*
CFF/Type 1:
1. Can encode as long as there is enough room in the encoding array [256 max, not including the required 0 place for notdef.]
2. While encoding try using the WinAnsiEncoding encoding if possible for the relevant char-code value.
Meaning, translate the Unicode value to the matching WinAnsiEncoding value. 
If no matching value found (character is not in win ANSI) use some value, giving preference to non-WinAnsiEncoding value.
3. While writing the font description dictionaries use the FONTs glyph names to write the differences array.
It should have something. Otherwise trace for now. I might have to write my own glyph mapping method to 
give the font intended glyph names (as oppose as using FreeTypes, which might not be complete. Should check the bloody code)

True Type:
1. Can encoding if/f all text codes are available through WinAnsiEncoding.  
[maybe should also make sure that the font has the relevant cmaps?! Or maybe I'm just assuming that...]
2. While encoding use WinAnsiEncoding values, of course. This will necasserily work
3. While writing the font description simply write the WinAnsiEncoding glyph name, and pray.
*/

void AbstractWrittenFont::AppendGlyphs(	const GlyphUnicodeMappingListList& inGlyphsList,
										UShortListList& outEncodedCharacters,
										bool& outEncodingIsMultiByte,
										ObjectIDType &outFontObjectID)
{
	// same as the regular one, but with lists of strings

	if(mCIDRepresentation && CanEncodeWithIncludedChars(mCIDRepresentation,inGlyphsList,outEncodedCharacters))
	{
		outFontObjectID = mCIDRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = true;
		return;
	}

	if(mANSIRepresentation && CanEncodeWithIncludedChars(mANSIRepresentation,inGlyphsList,outEncodedCharacters))
	{
		outFontObjectID = mANSIRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = false;
		return;
	}

	if(mCIDRepresentation)
	{
		AddToCIDRepresentation(inGlyphsList,outEncodedCharacters);
		outFontObjectID = mCIDRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = true;
		return;
	}

	if(!mANSIRepresentation)
		mANSIRepresentation = new WrittenFontRepresentation();

	if(AddToANSIRepresentation(inGlyphsList,outEncodedCharacters))
	{
		if(0 == mANSIRepresentation->mWrittenObjectID)
			mANSIRepresentation->mWrittenObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

		outFontObjectID = mANSIRepresentation->mWrittenObjectID;
		outEncodingIsMultiByte = false;
		return;			
	}

	mCIDRepresentation = new WrittenFontRepresentation();
	AddToCIDRepresentation(inGlyphsList,outEncodedCharacters);
	outFontObjectID = mCIDRepresentation->mWrittenObjectID;
	outEncodingIsMultiByte = true;
}

bool AbstractWrittenFont::CanEncodeWithIncludedChars(	WrittenFontRepresentation* inRepresentation, 
														const GlyphUnicodeMappingListList& inGlyphsList,
														UShortListList& outEncodedCharacters)
{
	UShortListList candidateEncodingList;
	UShortList candidateEncoding;
	GlyphUnicodeMappingListList::const_iterator it=inGlyphsList.begin();
	GlyphUnicodeMappingList::const_iterator itGlyphs;
	UIntToGlyphEncodingInfoMap::iterator itEncoding;
	bool allIncluded = true;

	for(; it != inGlyphsList.end() && allIncluded; ++it)
	{
		itGlyphs = it->begin();
		for(; itGlyphs != it->end() && allIncluded; ++itGlyphs)
		{
			itEncoding = inRepresentation->mGlyphIDToEncodedChar.find(itGlyphs->mGlyphCode);
			if(itEncoding == inRepresentation->mGlyphIDToEncodedChar.end())
				allIncluded = false;
			else
				candidateEncoding.push_back(itEncoding->second.mEncodedCharacter);
		}
		candidateEncodingList.push_back(candidateEncoding);
		candidateEncoding.clear();
	}

	if(allIncluded)
		outEncodedCharacters = candidateEncodingList;
	return allIncluded;
}

void AbstractWrittenFont::AddToCIDRepresentation(	const GlyphUnicodeMappingListList& inGlyphsList,
													UShortListList& outEncodedCharacters)
{
	// Glyph IDs are always used as CIDs, there's a possible @#$@up here if the font will contain too many glyphs...oops.
	// take care of this sometimes.

	// for the first time, add also 0,0 mapping
	if(mCIDRepresentation->mGlyphIDToEncodedChar.size() == 0)
		mCIDRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


	GlyphUnicodeMappingListList::const_iterator itList = inGlyphsList.begin();
	GlyphUnicodeMappingList::const_iterator it;
	UIntToGlyphEncodingInfoMap::iterator itEncoding;
	UShortList encodedCharacters;

	for(; itList != inGlyphsList.end(); ++itList)
	{
		it = itList->begin();
		for(; it != itList->end(); ++it)
		{
			itEncoding = mCIDRepresentation->mGlyphIDToEncodedChar.find(it->mGlyphCode);
			if(itEncoding == mCIDRepresentation->mGlyphIDToEncodedChar.end())
			{
				itEncoding = mCIDRepresentation->mGlyphIDToEncodedChar.insert(
					UIntToGlyphEncodingInfoMap::value_type(it->mGlyphCode,GlyphEncodingInfo(it->mGlyphCode,it->mUnicodeValues))).first;
			}
			encodedCharacters.push_back(itEncoding->second.mEncodedCharacter);
		}
		outEncodedCharacters.push_back(encodedCharacters);
		encodedCharacters.clear();
	}

	if(0 == mCIDRepresentation->mWrittenObjectID)
		mCIDRepresentation->mWrittenObjectID = mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
}
#include "WrittenFontTrueType.h"
#include "WinAnsiEncoding.h"
#include "TrueTypeANSIFontWriter.h"
#include "Trace.h"
#include "TrueTypeDescendentFontWriter.h"
#include "CIDFontWriter.h"

WrittenFontTrueType::WrittenFontTrueType(ObjectsContext* inObjectsContext):AbstractWrittenFont(inObjectsContext)
{
}

WrittenFontTrueType::~WrittenFontTrueType(void)
{
}

/*
here's what i'm deciding on:
1. Can encoding if/f all text codes are available through WinAnsiEncoding.  
[maybe should also make sure that the font has the relevant cmaps?! Or maybe I'm just assuming that...]
2. While encoding use WinAnsiEncoding values, of course. This will necasserily work
3. While writing the font description simply write the WinAnsiEncoding glyph name, and pray.*/

bool WrittenFontTrueType::AddToANSIRepresentation(	const wstring& inText,
													const UIntList& inGlyphsList,
													UShortList& outEncodedCharacters)
{
	// i'm totally relying on the text here, which is fine till i'll do ligatures, in which case
	// i'll need to make something different out of the text.
	// as you can see this has little to do with glyphs (mainly cause i can't use FreeType to map the glyphs
	// back to the rleevant unicode values...but no need anyways...that's why i carry the text).
	UShortList candidates;
	BoolAndByte encodingResult(true,0);
	WinAnsiEncoding winAnsiEncoding;
	wstring::const_iterator it = inText.begin(); 

	for(; it != inText.end() && encodingResult.first; ++it)
	{
		encodingResult = winAnsiEncoding.Encode(*it);
		if(encodingResult.first)
			candidates.push_back(encodingResult.second);
	}

	if(encodingResult.first)
	{
		// for the first time, add also 0,0 mapping
		if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
			mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


		UIntList::const_iterator itGlyphs = inGlyphsList.begin();
		UShortList::iterator itEncoded = candidates.begin();
		wstring::const_iterator itText = inText.begin(); 
		for(; itGlyphs != inGlyphsList.end(); ++ itGlyphs,++itEncoded,++itText)
		{
			if(mANSIRepresentation->mGlyphIDToEncodedChar.find(*itGlyphs) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
				mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(*itGlyphs,GlyphEncodingInfo(*itEncoded,*itText)));
		}

		outEncodedCharacters = candidates;
	}

	return encodingResult.first;
}


EStatusCode WrittenFontTrueType::WriteFontDefinition(FreeTypeFaceWrapper& inFontInfo)
{
	EStatusCode status = eSuccess;
	do
	{
		if(mANSIRepresentation && mANSIRepresentation->mWrittenObjectID != 0)
		{
			TrueTypeANSIFontWriter fontWriter;

			status = fontWriter.WriteFont(inFontInfo,mANSIRepresentation,mObjectsContext);
			if(status != eSuccess)
			{
				TRACE_LOG("WrittenFontTrueType::WriteFontDefinition, Failed to write Ansi font definition");
				break;

			}
		}

		if(mCIDRepresentation && mCIDRepresentation->mWrittenObjectID != 0)
		{
			CIDFontWriter fontWriter;
			TrueTypeDescendentFontWriter descendentFontWriter;

			status = fontWriter.WriteFont(inFontInfo,mCIDRepresentation,mObjectsContext,&descendentFontWriter);
			if(status != eSuccess)
			{
				TRACE_LOG("WrittenFontTrueType::WriteFontDefinition, Failed to write CID font definition");
				break;
			}
		}

	} while(false);

	return status;
}

bool WrittenFontTrueType::AddToANSIRepresentation(	const WStringList& inText,
													const UIntListList& inGlyphsList,
													UShortListList& outEncodedCharacters)
{
	UShortListList candidatesList;
	UShortList candidates;
	BoolAndByte encodingResult(true,0);
	WinAnsiEncoding winAnsiEncoding;
	WStringList::const_iterator itList = inText.begin(); 
	wstring::const_iterator it; 

	for(; itList != inText.end() && encodingResult.first; ++itList)
	{
		it = itList->begin();
		for(; it != itList->end() && encodingResult.first; ++it)
		{
			encodingResult = winAnsiEncoding.Encode(*it);
			if(encodingResult.first)
				candidates.push_back(encodingResult.second);
		}
		if(encodingResult.first)
		{
			candidatesList.push_back(candidates);
			candidates.clear();
		}
	}

	if(encodingResult.first)
	{
		// for the first time, add also 0,0 mapping
		if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
			mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


		UIntListList::const_iterator itGlyphsList = inGlyphsList.begin();
		UShortListList::iterator itEncodedList = candidatesList.begin();
		WStringList::const_iterator itTextList = inText.begin();
		UIntList::const_iterator itGlyphs;
		UShortList::iterator itEncoded;
		wstring::const_iterator itText;

		for(; itGlyphsList != inGlyphsList.end(); ++ itGlyphsList,++itEncodedList,++itTextList)
		{
			itGlyphs = itGlyphsList->begin();
			itEncoded = itEncodedList->begin();
			itText = itTextList->begin();
			for(; itGlyphs != itGlyphsList->end(); ++ itGlyphs,++itEncoded,++itText)
			{
				if(mANSIRepresentation->mGlyphIDToEncodedChar.find(*itGlyphs) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
					mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(*itGlyphs,GlyphEncodingInfo(*itEncoded,*itText)));
			}
		}

		outEncodedCharacters = candidatesList;
	}

	return encodingResult.first;	
}
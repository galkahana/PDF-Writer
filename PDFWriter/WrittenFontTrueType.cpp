#include "WrittenFontTrueType.h"
#include "WinAnsiEncoding.h"

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
		outEncodedCharacters = candidates;

	return encodingResult.first;
}

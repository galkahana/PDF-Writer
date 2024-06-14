/*
   Source File : WrittenFontTrueType.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "WrittenFontTrueType.h"
#include "WinAnsiEncoding.h"
#include "TrueTypeANSIFontWriter.h"
#include "Trace.h"
#include "TrueTypeDescendentFontWriter.h"
#include "CIDFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "PDFObjectCast.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "FreeTypeFaceWrapper.h"

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace PDFHummus;

bool FontHasCmapsForWinAnsiEncoding(FT_Face font) {
	// See PDF Reference 5.5.5 Character Encoding, Encodings for TrueType Fonts. When is mapping from glyph name is possible, and so non CID encoding can be built - only if font contains either of 
	// two possible cmaps: win unicode bmp or macintosh roman. so check if got either of those.
	for(FT_Int i = 0; i < font->num_charmaps; ++i) {
		FT_CharMap charmap = font->charmaps[i];

		if (charmap->platform_id == 3 && charmap->encoding_id == 1)
			return true; // Windows Unicode BMP

		if (charmap->platform_id == 1 && charmap->encoding_id == 0)
			return true; // Macintosh Roman

	}

	return false;
}

WrittenFontTrueType::WrittenFontTrueType(ObjectsContext* inObjectsContext, FreeTypeFaceWrapper* inFontInfo):AbstractWrittenFont(inObjectsContext, inFontInfo)
{
	fontSupportsWinAnsiEncoding = FontHasCmapsForWinAnsiEncoding(*inFontInfo);
}

WrittenFontTrueType::~WrittenFontTrueType(void)
{
}

/*
here's what i'm deciding on:
1. Can encoding if/f all text codes are available through WinAnsiEncoding.  
[maybe should also make sure that the font has the relevant cmaps?! Or maybe I'm just assuming that...] [That's what FontHasCmapsForWinAnsiEncoding and fontSupportsWinAnsiEncoding are for]
2. While encoding use WinAnsiEncoding values, of course. This will necasserily work
3. While writing the font description simply write the WinAnsiEncoding glyph name, and pray.*/

bool WrittenFontTrueType::AddANSICandidates(const GlyphUnicodeMappingList& inGlyphsList, UShortList& ioCandidates) {
	if(!fontSupportsWinAnsiEncoding) {
		return false;
	}

	// i'm totally relying on the text here, which is fine till i'll do ligatures, in which case
	// i'll need to make something different out of the text.
	// as you can see this has little to do with glyphs (mainly cause i can't use FreeType to map the glyphs
	// back to the rleevant unicode values...but no need anyways...that's why i carry the text).
	BoolAndByte encodingResult(true,0);
	WinAnsiEncoding winAnsiEncoding;
	GlyphUnicodeMappingList::const_iterator it = inGlyphsList.begin(); 

	for(; it != inGlyphsList.end() && encodingResult.first; ++it)
	{
		// don't bother with characters of more (or less) than one unicode
		if(it->mUnicodeValues.size() != 1)
		{
			encodingResult.first = false;
		}
		else if(0x2022 == it->mUnicodeValues.front())
		{
			// From the reference:
			// In WinAnsiEncoding, all unused codes greater than 40 map to the bullet character. 
			// However, only code 225 is specifically assigned to the bullet character; other codes are subject to future reassignment.

			// now i don't know if it's related or not...but acrobat isn't happy when i'm using winansi with bullet. and text coming after that bullet may be
			// corrupted.
			// so i'm forcing CID if i hit bullet till i know better.
			encodingResult.first = false;
		}
		else
		{
			encodingResult = winAnsiEncoding.Encode(it->mUnicodeValues.front());
			if(encodingResult.first)
				ioCandidates.push_back(encodingResult.second);
		}
	}

	return encodingResult.first;
}

bool WrittenFontTrueType::AddToANSIRepresentation(const GlyphUnicodeMappingList& inGlyphsList, UShortList& outEncodedCharacters)
{
	UShortList candidates;

	bool result = AddANSICandidates(inGlyphsList, candidates);

	if(result)
	{
		// for the first time, add also 0,0 mapping
		if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
			mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


		GlyphUnicodeMappingList::const_iterator itGlyphs = inGlyphsList.begin();
		UShortList::iterator itEncoded = candidates.begin();
		for(; itGlyphs != inGlyphsList.end(); ++ itGlyphs,++itEncoded)
		{
			if(mANSIRepresentation->mGlyphIDToEncodedChar.find(itGlyphs->mGlyphCode) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
				mANSIRepresentation->mGlyphIDToEncodedChar.insert(
					UIntToGlyphEncodingInfoMap::value_type(itGlyphs->mGlyphCode,GlyphEncodingInfo(*itEncoded,itGlyphs->mUnicodeValues)));
		}

		outEncodedCharacters = candidates;
	}

	return result;
}


EStatusCode WrittenFontTrueType::WriteFontDefinition(bool inEmbedFont)
{
	EStatusCode status = PDFHummus::eSuccess;
	do
	{
		if(mANSIRepresentation && !mANSIRepresentation->isEmpty() && mANSIRepresentation->mWrittenObjectID != 0)
		{
			TrueTypeANSIFontWriter fontWriter;

			status = fontWriter.WriteFont(*mFontInfo, mANSIRepresentation, mObjectsContext, inEmbedFont);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("WrittenFontTrueType::WriteFontDefinition, Failed to write Ansi font definition");
				break;

			}
		}

		if(mCIDRepresentation && !mCIDRepresentation->isEmpty()  && mCIDRepresentation->mWrittenObjectID != 0)
		{
			CIDFontWriter fontWriter;
			TrueTypeDescendentFontWriter descendentFontWriter;

			status = fontWriter.WriteFont(*mFontInfo, mCIDRepresentation, mObjectsContext, &descendentFontWriter, inEmbedFont);
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("WrittenFontTrueType::WriteFontDefinition, Failed to write CID font definition");
				break;
			}
		}

	} while(false);

	return status;
}

bool WrittenFontTrueType::AddToANSIRepresentation(	const GlyphUnicodeMappingListList& inGlyphsList,
													UShortListList& outEncodedCharacters)
{
	UShortListList candidatesList;
	UShortList candidates;
	BoolAndByte encodingResult(true,0);
	GlyphUnicodeMappingListList::const_iterator itList = inGlyphsList.begin(); 
	bool result = true;

	for(; itList != inGlyphsList.end() && result; ++itList)
	{
		result = AddANSICandidates(*itList, candidates);
		if(result)
		{
			candidatesList.push_back(candidates);
			candidates.clear();
		}
	}

	if(result)
	{
		// for the first time, add also 0,0 mapping
		if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
			mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));


		GlyphUnicodeMappingListList::const_iterator itGlyphsList = inGlyphsList.begin();
		UShortListList::iterator itEncodedList = candidatesList.begin();
		GlyphUnicodeMappingList::const_iterator itGlyphs;
		UShortList::iterator itEncoded;

		for(; itGlyphsList != inGlyphsList.end(); ++ itGlyphsList,++itEncodedList)
		{
			itGlyphs = itGlyphsList->begin();
			itEncoded = itEncodedList->begin();
			for(; itGlyphs != itGlyphsList->end(); ++ itGlyphs,++itEncoded)
			{
				if(mANSIRepresentation->mGlyphIDToEncodedChar.find(itGlyphs->mGlyphCode) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
					mANSIRepresentation->mGlyphIDToEncodedChar.insert(
					UIntToGlyphEncodingInfoMap::value_type(itGlyphs->mGlyphCode,GlyphEncodingInfo(*itEncoded,itGlyphs->mUnicodeValues)));
			}
		}

		outEncodedCharacters = candidatesList;
	}

	return result;	
}

EStatusCode WrittenFontTrueType::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	inStateWriter->StartNewIndirectObject(inObjectID);

	DictionaryContext* writtenFontDictionary = inStateWriter->StartDictionary();

	writtenFontDictionary->WriteKey("Type");
	writtenFontDictionary->WriteNameValue("WrittenFontTrueType");

	EStatusCode status = AbstractWrittenFont::WriteStateInDictionary(inStateWriter,writtenFontDictionary);
	if(PDFHummus::eSuccess == status)
	{
		inStateWriter->EndDictionary(writtenFontDictionary);
		inStateWriter->EndIndirectObject();

		status = AbstractWrittenFont::WriteStateAfterDictionary(inStateWriter);
	}
	return status;
}

EStatusCode WrittenFontTrueType::ReadState(PDFParser* inStateReader,ObjectIDType inObjectID)
{
	PDFObjectCastPtr<PDFDictionary> writtenFontState(inStateReader->ParseNewObject(inObjectID));

	return AbstractWrittenFont::ReadStateFromObject(inStateReader,writtenFontState.GetPtr());
}

unsigned short WrittenFontTrueType::EncodeCIDGlyph(unsigned int inGlyphId) {
	// Gal 26/8/2017: Most of the times, the glyph IDs are CIDs. this is to retain a few requirements of True type fonts, and the case of fonts when they are not embedded.
	// However, when CFF fonts are embedded, the matching code actually recreates a font from just the subset, and renumbers them based on the order
	// of them joining the font. Hence, we need a slight difference for this case, and an override is provided
	return (unsigned short)inGlyphId;
}
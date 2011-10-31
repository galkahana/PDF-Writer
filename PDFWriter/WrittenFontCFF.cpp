/*
   Source File : WrittenFontCFF.cpp


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
#include "WrittenFontCFF.h"
#include "Trace.h"
#include "CFFANSIFontWriter.h"
#include "CIDFontWriter.h"
#include "CFFDescendentFontWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFArray.h"
#include "PDFInteger.h"
#include "PDFBoolean.h"

using namespace PDFHummus;

WrittenFontCFF::WrittenFontCFF(ObjectsContext* inObjectsContext,bool inIsCID):AbstractWrittenFont(inObjectsContext)
{
	mAvailablePositionsCount = 255;
	mFreeList.push_back(UCharAndUChar(1,255));
	// 1st place is reserved for .notdef/0 glyph index. we'll use 0s in the array in all other places as indication for avialability
	for(int i=0;i<256;++i)
		mAssignedPositions[i] = 0;
	mIsCID = inIsCID;
}

WrittenFontCFF::~WrittenFontCFF(void)
{
}

bool WrittenFontCFF::AddToANSIRepresentation(
						const GlyphUnicodeMappingList& inGlyphsList,
						UShortList& outEncodedCharacters)
{
	// categorically do not allow an ANSI representation if the font is CID
	if(!mIsCID && HasEnoughSpaceForGlyphs(inGlyphsList))
	{
		GlyphUnicodeMappingList::const_iterator it = inGlyphsList.begin();

		for(; it != inGlyphsList.end(); ++it)
			outEncodedCharacters.push_back(EncodeGlyph(it->mGlyphCode,it->mUnicodeValues));
		return true;
	}
	else
		return false;
}

bool WrittenFontCFF::HasEnoughSpaceForGlyphs(const GlyphUnicodeMappingList& inGlyphsList)
{
	GlyphUnicodeMappingList::const_iterator it = inGlyphsList.begin();
	int glyphsToAddCount = 0;

	for(; it != inGlyphsList.end(); ++it)
		if(mANSIRepresentation->mGlyphIDToEncodedChar.find(it->mGlyphCode) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
			++glyphsToAddCount;

	return glyphsToAddCount <= mAvailablePositionsCount;
}

unsigned short WrittenFontCFF::EncodeGlyph(unsigned int inGlyph,const ULongVector& inCharacters)
{
	// for the first time, add also 0,0 mapping
	if(mANSIRepresentation->mGlyphIDToEncodedChar.size() == 0)
		mANSIRepresentation->mGlyphIDToEncodedChar.insert(UIntToGlyphEncodingInfoMap::value_type(0,GlyphEncodingInfo(0,0)));

	UIntToGlyphEncodingInfoMap::iterator it = mANSIRepresentation->mGlyphIDToEncodedChar.find(inGlyph);

	if(it == mANSIRepresentation->mGlyphIDToEncodedChar.end())
	{
		// as a default position, i'm grabbing the ansi bits. this should display nice charachters, when possible
		unsigned char encoding;
		if(inCharacters.size() > 0)
			encoding = (unsigned char)(inCharacters.back() & 0xff);
		else
			encoding = (unsigned char)(inGlyph & 0xff);
		if(mAssignedPositions[encoding] == 0)
			RemoveFromFreeList(encoding);
		else
			encoding = AllocateFromFreeList(inGlyph);
		mAssignedPositions[encoding] = inGlyph;
		it = mANSIRepresentation->mGlyphIDToEncodedChar.insert(
				UIntToGlyphEncodingInfoMap::value_type(inGlyph,GlyphEncodingInfo(encoding,inCharacters))).first;
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
	EStatusCode status = PDFHummus::eSuccess;
	do
	{
		if(mANSIRepresentation && mANSIRepresentation->mWrittenObjectID != 0)
		{
			CFFANSIFontWriter fontWriter;

			status = fontWriter.WriteFont(inFontInfo,mANSIRepresentation,mObjectsContext);
			if(status != PDFHummus::eSuccess)
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
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("WrittenFontCFF::WriteFontDefinition, Failed to write CID font definition");
				break;
			}
		}

	} while(false);

	return status;
}

bool WrittenFontCFF::AddToANSIRepresentation(	const GlyphUnicodeMappingListList& inGlyphsList,
												UShortListList& outEncodedCharacters)
{
	// categorically do not allow an ANSI representation if the font is CID
	if(!mIsCID && HasEnoughSpaceForGlyphs(inGlyphsList))
	{
		GlyphUnicodeMappingListList::const_iterator itList = inGlyphsList.begin();
		GlyphUnicodeMappingList::const_iterator it;
		UShortList encodedCharacters;

		for(; itList != inGlyphsList.end(); ++itList)
		{
			it = itList->begin();
			for(; it != itList->end(); ++it)
				encodedCharacters.push_back(EncodeGlyph(it->mGlyphCode,it->mUnicodeValues));
			outEncodedCharacters.push_back(encodedCharacters);
			encodedCharacters.clear();

		}
		return true;
	}
	else
		return false;
}

bool WrittenFontCFF::HasEnoughSpaceForGlyphs(const GlyphUnicodeMappingListList& inGlyphsList)
{
	GlyphUnicodeMappingListList::const_iterator itList = inGlyphsList.begin();
	GlyphUnicodeMappingList::const_iterator it;
	int glyphsToAddCount = 0;

	for(; itList != inGlyphsList.end(); ++itList)
	{
		it = itList->begin();
		for(; it != itList->end(); ++it)
			if(mANSIRepresentation->mGlyphIDToEncodedChar.find(it->mGlyphCode) == mANSIRepresentation->mGlyphIDToEncodedChar.end())
				++glyphsToAddCount;
	}

	return glyphsToAddCount <= mAvailablePositionsCount;
}

EStatusCode WrittenFontCFF::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	inStateWriter->StartNewIndirectObject(inObjectID);

	DictionaryContext* writtenFontDictionary = inStateWriter->StartDictionary();

	writtenFontDictionary->WriteKey("Type");
	writtenFontDictionary->WriteNameValue("WrittenFontCFF");

	writtenFontDictionary->WriteKey("mAvailablePositionsCount");
	writtenFontDictionary->WriteIntegerValue(mAvailablePositionsCount);

	writtenFontDictionary->WriteKey("mFreeList");

	inStateWriter->StartArray();
	UCharAndUCharList::iterator it = mFreeList.begin();
	for(; it != mFreeList.end();++it)
	{
		inStateWriter->WriteInteger(it->first);
		inStateWriter->WriteInteger(it->second);
	}
	inStateWriter->EndArray(eTokenSeparatorEndLine);

	writtenFontDictionary->WriteKey("mAssignedPositions");
	inStateWriter->StartArray();
	for(int i=0;i<256;++i)
		inStateWriter->WriteInteger(mAssignedPositions[i]);
	inStateWriter->EndArray(eTokenSeparatorEndLine);

	writtenFontDictionary->WriteKey("mIsCID");
	writtenFontDictionary->WriteBooleanValue(mIsCID);

	EStatusCode status = AbstractWrittenFont::WriteStateInDictionary(inStateWriter,writtenFontDictionary);
	if(PDFHummus::eSuccess == status)
	{
		inStateWriter->EndDictionary(writtenFontDictionary);
		inStateWriter->EndIndirectObject();

		status = AbstractWrittenFont::WriteStateAfterDictionary(inStateWriter);
	}
	return status;
}

EStatusCode WrittenFontCFF::ReadState(PDFParser* inStateReader,ObjectIDType inObjectID)
{
	PDFObjectCastPtr<PDFDictionary> writtenFontState(inStateReader->ParseNewObject(inObjectID));


	PDFObjectCastPtr<PDFInteger> availablePositionsCount(writtenFontState->QueryDirectObject("mAvailablePositionsCount"));

	mAvailablePositionsCount = (unsigned char)availablePositionsCount->GetValue();

	mFreeList.clear();
	PDFObjectCastPtr<PDFArray> freeListState(writtenFontState->QueryDirectObject("mFreeList"));

	SingleValueContainerIterator<PDFObjectVector> it =  freeListState->GetIterator();
	PDFObjectCastPtr<PDFInteger> item;
	UCharAndUChar aPair;
	while(it.MoveNext())
	{
		item = it.GetItem();
		aPair.first = (unsigned char)item->GetValue();
		it.MoveNext();
		item = it.GetItem();
		aPair.second = (unsigned char)item->GetValue();
		mFreeList.push_back(aPair);
	}

	PDFObjectCastPtr<PDFArray> assignedPositionsState(writtenFontState->QueryDirectObject("mAssignedPositions"));
	it =  assignedPositionsState->GetIterator();
	int i=0;

	PDFObjectCastPtr<PDFInteger> assignedPositionItem;
	while(it.MoveNext())
	{
		assignedPositionItem = it.GetItem();
		mAssignedPositions[i] = (unsigned int)assignedPositionItem->GetValue();
		++i;
	}

	PDFObjectCastPtr<PDFBoolean> isCIDState(writtenFontState->QueryDirectObject("mIsCID"));

	mIsCID = isCIDState->GetValue();

	return AbstractWrittenFont::ReadState(inStateReader,writtenFontState.GetPtr());
}
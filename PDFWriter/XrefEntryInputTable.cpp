/*
   Source File : XrefEntryInputTable.cpp


   Copyright 2026 Gal Kahana PDFWriter

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
#include "XrefEntryInputTable.h"
#include "Trace.h"

using namespace PDFHummus;

// xref size is the trailer /Size, an integer. ISO 32000 Annex C puts integers within 32 bits
#define MAX_XREF_SIZE 2147483647

#define XREF_BLOCK_SIZE_BITS 10
#define XREF_BLOCK_SIZE (1 << XREF_BLOCK_SIZE_BITS)
#define XREF_BLOCK_INDEX_MASK (XREF_BLOCK_SIZE - 1)

static XrefEntryInput scUndefinedEntry;

XrefEntryInputTable::XrefEntryInputTable(void)
{
	mSize = 0;
}

XrefEntryInputTable::~XrefEntryInputTable(void)
{
	Reset();
}

void XrefEntryInputTable::Reset()
{
	XrefEntryInputPtrVector::iterator it = mBlocks.begin();
	for(; it != mBlocks.end(); ++it)
		delete[] *it;
	mBlocks.clear();
	mSize = 0;
}

XrefEntryInput* XrefEntryInputTable::GetEntry(ObjectIDType inObjectID)
{
	if(inObjectID >= mSize)
		return NULL;

	// the size may extend past the directory, e.g. after merging a table whose trailing entries are undefined
	ObjectIDType blockIndex = inObjectID >> XREF_BLOCK_SIZE_BITS;
	XrefEntryInput* block = blockIndex < mBlocks.size() ? mBlocks[blockIndex] : NULL;
	return block ? &(block[inObjectID & XREF_BLOCK_INDEX_MASK]) : &scUndefinedEntry;
}

EStatusCode XrefEntryInputTable::GetOrCreateEntry(ObjectIDType inObjectID, XrefEntryInput*& outEntry)
{
	if(inObjectID >= MAX_XREF_SIZE)
	{
		TRACE_LOG1("XrefEntryInputTable::GetOrCreateEntry, object ID %lu exceeds maximum xref size", (unsigned long)inObjectID);
		return eFailure;
	}

	ObjectIDType blockIndex = inObjectID >> XREF_BLOCK_SIZE_BITS;
	if(blockIndex >= mBlocks.size())
		mBlocks.resize(blockIndex + 1, NULL);
	if(!mBlocks[blockIndex])
		mBlocks[blockIndex] = new XrefEntryInput[XREF_BLOCK_SIZE];

	if(inObjectID >= mSize)
		mSize = inObjectID + 1;

	outEntry = &(mBlocks[blockIndex][inObjectID & XREF_BLOCK_INDEX_MASK]);
	return eSuccess;
}

ObjectIDType XrefEntryInputTable::GetSize()
{
	return mSize;
}

EStatusCode XrefEntryInputTable::MergeFrom(XrefEntryInputTable& inOther)
{
	EStatusCode status = eSuccess;

	for(ObjectIDType blockIndex = 0; blockIndex < inOther.mBlocks.size() && eSuccess == status; ++blockIndex)
	{
		XrefEntryInput* otherBlock = inOther.mBlocks[blockIndex];
		if(!otherBlock)
			continue;

		for(ObjectIDType i = 0; i < XREF_BLOCK_SIZE; ++i)
		{
			if(otherBlock[i].mType == eXrefEntryUndefined)
				continue;

			XrefEntryInput* entry;
			status = GetOrCreateEntry((blockIndex << XREF_BLOCK_SIZE_BITS) + i, entry);
			if(status != eSuccess)
				break;
			*entry = otherBlock[i];
		}
	}

	if(eSuccess == status && inOther.mSize > mSize)
		mSize = inOther.mSize;

	return status;
}

/*
   Source File : XrefEntryInputTable.h


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
#pragma once

#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"

#include <vector>

enum EXrefEntryType
{
	eXrefEntryExisting,
	eXrefEntryDelete,
	eXrefEntryStreamObject,
	eXrefEntryUndefined
};

struct XrefEntryInput
{
	XrefEntryInput(){mObjectPosition = 0;mRivision=0;mType = eXrefEntryUndefined;}

	// well...it's more like...the first number in a pair on an xref, and the second one. the names
	// are true only for "n" type of entries
	IOBasicTypes::LongFilePositionType mObjectPosition;
	unsigned long mRivision;
	EXrefEntryType mType;
};

typedef std::vector<XrefEntryInput*> XrefEntryInputPtrVector;

/*
	Xref entries by object ID. Entries are stored in fixed size blocks, allocated only when an entry
	in them is written, so a high object ID does not require allocating all entries below it.
	Same idea as an OS page table: the object ID splits into a block index and an offset in the block.
*/
class XrefEntryInputTable
{
public:
	XrefEntryInputTable(void);
	~XrefEntryInputTable(void);

	// Returns NULL for object IDs at or beyond GetSize(). Entries within the size that were never written
	// return a shared undefined entry, which must not be modified.
	XrefEntryInput* GetEntry(ObjectIDType inObjectID);

	// Returns a writable entry for the object ID, extending the size to include it if required
	PDFHummus::EStatusCode GetOrCreateEntry(ObjectIDType inObjectID, XrefEntryInput*& outEntry);

	// One past the highest object ID written
	ObjectIDType GetSize();

	// Copies the defined entries of inOther over this table's entries, and extends the size to inOther's size
	PDFHummus::EStatusCode MergeFrom(XrefEntryInputTable& inOther);

	void Reset();

private:
	XrefEntryInputPtrVector mBlocks;
	ObjectIDType mSize;

	// not copyable (blocks are owned). declared without implementation
	XrefEntryInputTable(const XrefEntryInputTable&);
	XrefEntryInputTable& operator=(const XrefEntryInputTable&);
};

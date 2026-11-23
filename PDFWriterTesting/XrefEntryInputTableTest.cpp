/*
   Source File : XrefEntryInputTableTest.cpp


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
#include "EStatusCode.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

// one below MAX_XREF_SIZE (2^31 - 1), the highest object ID the table accepts
static const ObjectIDType scHighestAcceptedObjectID = 2147483646;

static bool WriteEntry(XrefEntryInputTable& inTable, ObjectIDType inObjectID, IOBasicTypes::LongFilePositionType inPosition)
{
    XrefEntryInput* entry;
    if(inTable.GetOrCreateEntry(inObjectID, entry) != eSuccess)
        return false;
    entry->mObjectPosition = inPosition;
    entry->mRivision = 0;
    entry->mType = eXrefEntryExisting;
    return true;
}

// ---- GetOrCreateEntry cases -------------------------------------------------

struct GetOrCreateEntryCase {
    const char* label;
    ObjectIDType objectID;
    EStatusCode expectedStatus;
    ObjectIDType expectedSize;
};

static const GetOrCreateEntryCase scGetOrCreateEntryCases[] = {
    {"HighestAcceptedID_ReturnsSuccess", scHighestAcceptedObjectID,     eSuccess, scHighestAcceptedObjectID + 1},
    {"IDAtMaxXrefSize_ReturnsFailure",   scHighestAcceptedObjectID + 1, eFailure, 0},
};

static bool RunGetOrCreateEntryCases()
{
    const size_t count = sizeof(scGetOrCreateEntryCases) / sizeof(scGetOrCreateEntryCases[0]);

    for(size_t i = 0; i < count; ++i) {
        const GetOrCreateEntryCase& c = scGetOrCreateEntryCases[i];

        // Arrange
        XrefEntryInputTable table;
        XrefEntryInput* entry = NULL;

        // Act
        EStatusCode status = table.GetOrCreateEntry(c.objectID, entry);

        // Assert
        if(status != c.expectedStatus || table.GetSize() != c.expectedSize) {
            cout << "XrefEntryInputTableTest [GetOrCreateEntry::" << c.label << "]: expected status "
                 << c.expectedStatus << " and size " << c.expectedSize << ", got status " << status
                 << " and size " << table.GetSize() << endl;
            return false;
        }
    }
    return true;
}

// ---- GetEntry ---------------------------------------------------------------

static bool GetEntry_HighObjectIDWritten_ReturnsWrittenEntry()
{
    // Arrange
    XrefEntryInputTable table;
    if(!WriteEntry(table, 2000000000, 1234)) {
        cout << "XrefEntryInputTableTest [GetEntry_HighObjectIDWritten_ReturnsWrittenEntry]: failed to write entry" << endl;
        return false;
    }

    // Act
    XrefEntryInput* entry = table.GetEntry(2000000000);

    // Assert
    if(!entry || entry->mType != eXrefEntryExisting || entry->mObjectPosition != 1234) {
        cout << "XrefEntryInputTableTest [GetEntry_HighObjectIDWritten_ReturnsWrittenEntry]: entry not as written" << endl;
        return false;
    }
    return true;
}

static bool GetEntry_UnwrittenIDWithinSize_ReturnsUndefinedEntry()
{
    // Arrange
    XrefEntryInputTable table;
    if(!WriteEntry(table, 5000, 1234)) {
        cout << "XrefEntryInputTableTest [GetEntry_UnwrittenIDWithinSize_ReturnsUndefinedEntry]: failed to write entry" << endl;
        return false;
    }

    // Act
    XrefEntryInput* entry = table.GetEntry(10);

    // Assert
    if(!entry || entry->mType != eXrefEntryUndefined || entry->mObjectPosition != 0) {
        cout << "XrefEntryInputTableTest [GetEntry_UnwrittenIDWithinSize_ReturnsUndefinedEntry]: expected an undefined entry" << endl;
        return false;
    }
    return true;
}

static bool GetEntry_IDAtSize_ReturnsNull()
{
    // Arrange
    XrefEntryInputTable table;
    if(!WriteEntry(table, 10, 1234)) {
        cout << "XrefEntryInputTableTest [GetEntry_IDAtSize_ReturnsNull]: failed to write entry" << endl;
        return false;
    }

    // Act
    XrefEntryInput* entry = table.GetEntry(11);

    // Assert
    if(entry != NULL) {
        cout << "XrefEntryInputTableTest [GetEntry_IDAtSize_ReturnsNull]: expected NULL" << endl;
        return false;
    }
    return true;
}

// ---- MergeFrom --------------------------------------------------------------

static bool MergeFrom_EntriesAcrossBlocks_CopiesOnlyDefinedEntries()
{
    // Arrange - target has entries at 3 and 2000. source overrides 2000, adds 5000, and has no entry at 3
    XrefEntryInputTable target;
    XrefEntryInputTable source;
    if(!WriteEntry(target, 3, 100) || !WriteEntry(target, 2000, 200) ||
       !WriteEntry(source, 2000, 999) || !WriteEntry(source, 5000, 300)) {
        cout << "XrefEntryInputTableTest [MergeFrom_EntriesAcrossBlocks_CopiesOnlyDefinedEntries]: failed to write entries" << endl;
        return false;
    }

    // Act
    EStatusCode status = target.MergeFrom(source);

    // Assert
    if(status != eSuccess ||
       target.GetSize() != 5001 ||
       target.GetEntry(3)->mObjectPosition != 100 ||
       target.GetEntry(2000)->mObjectPosition != 999 ||
       target.GetEntry(5000)->mObjectPosition != 300) {
        cout << "XrefEntryInputTableTest [MergeFrom_EntriesAcrossBlocks_CopiesOnlyDefinedEntries]: merged table not as expected" << endl;
        return false;
    }
    return true;
}

static bool MergeFrom_SourceWithOnlyUndefinedEntries_UnwrittenEntriesReturnUndefined()
{
    // Arrange - source entry 5000 is created but never given a type, so the merge copies no blocks
    XrefEntryInputTable target;
    XrefEntryInputTable source;
    XrefEntryInput* entry;
    if(source.GetOrCreateEntry(5000, entry) != eSuccess) {
        cout << "XrefEntryInputTableTest [MergeFrom_SourceWithOnlyUndefinedEntries_UnwrittenEntriesReturnUndefined]: failed to create entry" << endl;
        return false;
    }
    if(target.MergeFrom(source) != eSuccess) {
        cout << "XrefEntryInputTableTest [MergeFrom_SourceWithOnlyUndefinedEntries_UnwrittenEntriesReturnUndefined]: merge failed" << endl;
        return false;
    }

    // Act
    XrefEntryInput* mergedEntry = target.GetEntry(10);

    // Assert
    if(target.GetSize() != 5001 || !mergedEntry || mergedEntry->mType != eXrefEntryUndefined) {
        cout << "XrefEntryInputTableTest [MergeFrom_SourceWithOnlyUndefinedEntries_UnwrittenEntriesReturnUndefined]: expected size 5001 and an undefined entry" << endl;
        return false;
    }
    return true;
}

// ---- CountInUseEntries ------------------------------------------------------

static bool CountInUseEntries_MixedTypesAcrossBlocks_CountsExistingAndStreamObjectsOnly()
{
    // Arrange - existing at 1, stream object at 2000, deleted at 3000, created but undefined at 4000
    XrefEntryInputTable table;
    XrefEntryInput* streamObjectEntry;
    XrefEntryInput* deletedEntry;
    XrefEntryInput* undefinedEntry;
    if(!WriteEntry(table, 1, 100) ||
       table.GetOrCreateEntry(2000, streamObjectEntry) != eSuccess ||
       table.GetOrCreateEntry(3000, deletedEntry) != eSuccess ||
       table.GetOrCreateEntry(4000, undefinedEntry) != eSuccess) {
        cout << "XrefEntryInputTableTest [CountInUseEntries_MixedTypesAcrossBlocks_CountsExistingAndStreamObjectsOnly]: failed to write entries" << endl;
        return false;
    }
    streamObjectEntry->mType = eXrefEntryStreamObject;
    deletedEntry->mType = eXrefEntryDelete;

    // Act
    ObjectIDType count = table.CountInUseEntries();

    // Assert
    if(count != 2) {
        cout << "XrefEntryInputTableTest [CountInUseEntries_MixedTypesAcrossBlocks_CountsExistingAndStreamObjectsOnly]: expected 2, got " << count << endl;
        return false;
    }
    return true;
}

// ---- Reset ------------------------------------------------------------------

static bool Reset_TableWithEntries_EmptiesTable()
{
    // Arrange
    XrefEntryInputTable table;
    if(!WriteEntry(table, 3000, 1234)) {
        cout << "XrefEntryInputTableTest [Reset_TableWithEntries_EmptiesTable]: failed to write entry" << endl;
        return false;
    }

    // Act
    table.Reset();

    // Assert
    if(table.GetSize() != 0 || table.GetEntry(3000) != NULL) {
        cout << "XrefEntryInputTableTest [Reset_TableWithEntries_EmptiesTable]: expected an empty table" << endl;
        return false;
    }
    return true;
}

// ---- Entry point ------------------------------------------------------------

int XrefEntryInputTableTest(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    if(!RunGetOrCreateEntryCases()) return 1;
    if(!GetEntry_HighObjectIDWritten_ReturnsWrittenEntry()) return 1;
    if(!GetEntry_UnwrittenIDWithinSize_ReturnsUndefinedEntry()) return 1;
    if(!GetEntry_IDAtSize_ReturnsNull()) return 1;
    if(!MergeFrom_EntriesAcrossBlocks_CopiesOnlyDefinedEntries()) return 1;
    if(!MergeFrom_SourceWithOnlyUndefinedEntries_UnwrittenEntriesReturnUndefined()) return 1;
    if(!CountInUseEntries_MixedTypesAcrossBlocks_CountsExistingAndStreamObjectsOnly()) return 1;
    if(!Reset_TableWithEntries_EmptiesTable()) return 1;
    return 0;
}

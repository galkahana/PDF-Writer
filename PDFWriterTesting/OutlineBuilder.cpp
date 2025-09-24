/*
 Source File : OutlineBuilder.cpp


 Copyright 2025 Gal Kahana PDFWriter

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
#include "OutlineBuilder.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"

OutlineBuilder::OutlineBuilder()
{
}

OutlineBuilder::~OutlineBuilder()
{
}

EStatusCode OutlineBuilder::WriteOutlineItem(ObjectsContext* inObjectsContext, ObjectIDType inItemId,
                                           const OutlineItem& inItem, ObjectIDType inParentId, double inDefaultPageHeight,
                                           ObjectIDType inPrevId, ObjectIDType inNextId,
                                           ObjectIDType inFirstChildId, ObjectIDType inLastChildId)
{
    EStatusCode status = eSuccess;

    do
    {
        status = inObjectsContext->StartNewIndirectObject(inItemId);
        if(status != eSuccess)
            break;

        DictionaryContext* outlineItemDict = inObjectsContext->StartDictionary();

        outlineItemDict->WriteKey("Title");
        outlineItemDict->WriteLiteralStringValue(inItem.title);

        outlineItemDict->WriteKey("Parent");
        outlineItemDict->WriteObjectReferenceValue(inParentId);

        if(inPrevId != 0)
        {
            outlineItemDict->WriteKey("Prev");
            outlineItemDict->WriteObjectReferenceValue(inPrevId);
        }

        if(inNextId != 0)
        {
            outlineItemDict->WriteKey("Next");
            outlineItemDict->WriteObjectReferenceValue(inNextId);
        }

        if(inFirstChildId != 0)
        {
            outlineItemDict->WriteKey("First");
            outlineItemDict->WriteObjectReferenceValue(inFirstChildId);
        }

        if(inLastChildId != 0)
        {
            outlineItemDict->WriteKey("Last");
            outlineItemDict->WriteObjectReferenceValue(inLastChildId);
        }

        if(inItem.childCount > 0)
        {
            outlineItemDict->WriteKey("Count");
            outlineItemDict->WriteIntegerValue((long)inItem.childCount);
        }

        outlineItemDict->WriteKey("Dest");
        inObjectsContext->StartArray();
        inObjectsContext->WriteIndirectObjectReference(inItem.pageId);
        inObjectsContext->WriteName("XYZ");
        inObjectsContext->WriteInteger(0);
        // Use item-specific page height if set, otherwise use default
        double pageHeight = (inItem.pageHeight > 0.0) ? inItem.pageHeight : inDefaultPageHeight;
        inObjectsContext->WriteInteger((long)pageHeight);
        inObjectsContext->WriteInteger(0);
        inObjectsContext->EndArray();

        inObjectsContext->EndDictionary(outlineItemDict);
        inObjectsContext->EndIndirectObject();
    }
    while(false);

    return status;
}

size_t OutlineBuilder::CountTotalOutlineItems(const OutlineItem* inItems, size_t inItemCount)
{
    size_t total = inItemCount;
    for(size_t i = 0; i < inItemCount; ++i)
    {
        if(inItems[i].children && inItems[i].childCount > 0)
            total += CountTotalOutlineItems(inItems[i].children, inItems[i].childCount);
    }
    return total;
}

EStatusCode OutlineBuilder::WriteOutlineItems(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                            size_t inItemCount, ObjectIDType inParentId, double inDefaultPageHeight, ObjectIDType* inItemIds, size_t& ioItemIndex)
{
    EStatusCode status = eSuccess;

    // Pre-calculate positions for nextId calculation
    size_t* itemPositions = new size_t[inItemCount];
    size_t currentPos = ioItemIndex;

    for(size_t i = 0; i < inItemCount; ++i)
    {
        itemPositions[i] = currentPos;
        currentPos++; // for the item itself
        if(inItems[i].children && inItems[i].childCount > 0)
            currentPos += CountTotalOutlineItems(inItems[i].children, inItems[i].childCount);
    }

    for(size_t i = 0; i < inItemCount && status == eSuccess; ++i)
    {
        ObjectIDType currentItemId = inItemIds[ioItemIndex];

        // Calculate prev/next based on sibling positions
        ObjectIDType prevId = (i > 0) ? inItemIds[itemPositions[i-1]] : 0;
        ObjectIDType nextId = (i < inItemCount - 1) ? inItemIds[itemPositions[i+1]] : 0;

        ObjectIDType firstChildId = 0, lastChildId = 0;

        // If this item has children, calculate their IDs
        if(inItems[i].children && inItems[i].childCount > 0)
        {
            firstChildId = inItemIds[ioItemIndex + 1];
            // Calculate last child ID by skipping over all descendants
            size_t childrenTotal = CountTotalOutlineItems(inItems[i].children, inItems[i].childCount);
            lastChildId = inItemIds[ioItemIndex + childrenTotal];
        }

        status = WriteOutlineItem(inObjectsContext, currentItemId, inItems[i], inParentId, inDefaultPageHeight,
                                prevId, nextId, firstChildId, lastChildId);

        ioItemIndex++;

        // Write children recursively
        if(status == eSuccess && inItems[i].children && inItems[i].childCount > 0)
        {
            status = WriteOutlineItems(inObjectsContext, inItems[i].children, inItems[i].childCount,
                                     currentItemId, inDefaultPageHeight, inItemIds, ioItemIndex);
        }
    }

    delete[] itemPositions;
    return status;
}

EStatusCode OutlineBuilder::CreateOutlineTree(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                            size_t inItemCount, double inDefaultPageHeight, ObjectIDType& outOutlinesId)
{
    EStatusCode status = eSuccess;

    do
    {
        if(inItemCount == 0)
            break;

        // Pre-allocate outline object ID
        outOutlinesId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

        // Count total items and allocate all IDs
        size_t totalItems = CountTotalOutlineItems(inItems, inItemCount);
        ObjectIDType* itemIds = new ObjectIDType[totalItems];

        for(size_t i = 0; i < totalItems; ++i)
            itemIds[i] = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();

        // Write all outline items recursively
        size_t itemIndex = 0;
        status = WriteOutlineItems(inObjectsContext, inItems, inItemCount, outOutlinesId, inDefaultPageHeight, itemIds, itemIndex);

        if(status != eSuccess)
        {
            delete[] itemIds;
            break;
        }

        // Write outlines dictionary
        status = inObjectsContext->StartNewIndirectObject(outOutlinesId);
        if(status != eSuccess)
        {
            delete[] itemIds;
            break;
        }

        DictionaryContext* outlinesDict = inObjectsContext->StartDictionary();

        outlinesDict->WriteKey("Type");
        outlinesDict->WriteNameValue("Outlines");

        outlinesDict->WriteKey("First");
        outlinesDict->WriteObjectReferenceValue(itemIds[0]);

        // Calculate the position of the last top-level item
        size_t lastTopLevelPos = 0;
        for(size_t i = 0; i < inItemCount - 1; ++i)
        {
            lastTopLevelPos++; // for the item itself
            if(inItems[i].children && inItems[i].childCount > 0)
                lastTopLevelPos += CountTotalOutlineItems(inItems[i].children, inItems[i].childCount);
        }
        outlinesDict->WriteKey("Last");
        outlinesDict->WriteObjectReferenceValue(itemIds[lastTopLevelPos]);

        outlinesDict->WriteKey("Count");
        outlinesDict->WriteIntegerValue((long)inItemCount);

        inObjectsContext->EndDictionary(outlinesDict);
        inObjectsContext->EndIndirectObject();

        delete[] itemIds;
    }
    while(false);

    return status;
}

EStatusCode OutlineBuilder::CreateOutlineTree(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                            size_t inItemCount, ObjectIDType& outOutlinesId)
{
    return CreateOutlineTree(inObjectsContext, inItems, inItemCount, 0.0, outOutlinesId);
}
/*
 Source File : OutlineBuilder.h


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
#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include <string>

using namespace PDFHummus;

class ObjectsContext;

struct OutlineItem
{
    std::string title;
    ObjectIDType pageId;
    const OutlineItem* children;
    size_t childCount;
    double pageHeight;  // 0 = use default page height

    OutlineItem() : children(NULL), childCount(0), pageHeight(0.0) {}
    OutlineItem(const std::string& inTitle, ObjectIDType inPageId)
        : title(inTitle), pageId(inPageId), children(NULL), childCount(0), pageHeight(0.0) {}
    OutlineItem(const std::string& inTitle, ObjectIDType inPageId, const OutlineItem* inChildren, size_t inChildCount)
        : title(inTitle), pageId(inPageId), children(inChildren), childCount(inChildCount), pageHeight(0.0) {}
    OutlineItem(const std::string& inTitle, ObjectIDType inPageId, double inPageHeight)
        : title(inTitle), pageId(inPageId), children(NULL), childCount(0), pageHeight(inPageHeight) {}
    OutlineItem(const std::string& inTitle, ObjectIDType inPageId, const OutlineItem* inChildren, size_t inChildCount, double inPageHeight)
        : title(inTitle), pageId(inPageId), children(inChildren), childCount(inChildCount), pageHeight(inPageHeight) {}
};

class OutlineBuilder
{
public:
    OutlineBuilder();
    ~OutlineBuilder();

    EStatusCode CreateOutlineTree(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                size_t inItemCount, double inDefaultPageHeight, ObjectIDType& outOutlinesId);

    // Version with no default page height - all OutlineItems must have pageHeight set
    EStatusCode CreateOutlineTree(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                size_t inItemCount, ObjectIDType& outOutlinesId);

private:
    EStatusCode WriteOutlineItem(ObjectsContext* inObjectsContext, ObjectIDType inItemId,
                               const OutlineItem& inItem, ObjectIDType inParentId, double inDefaultPageHeight,
                               ObjectIDType inPrevId = 0, ObjectIDType inNextId = 0,
                               ObjectIDType inFirstChildId = 0, ObjectIDType inLastChildId = 0);

    size_t CountTotalOutlineItems(const OutlineItem* inItems, size_t inItemCount);

    EStatusCode WriteOutlineItems(ObjectsContext* inObjectsContext, const OutlineItem* inItems,
                                size_t inItemCount, ObjectIDType inParentId, double inDefaultPageHeight, ObjectIDType* inItemIds, size_t& ioItemIndex);
};
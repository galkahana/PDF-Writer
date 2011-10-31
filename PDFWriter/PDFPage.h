/*
   Source File : PDFPage.h


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
#pragma once

#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"
#include "SingleValueContainerIterator.h"
#include "ResourcesDictionary.h"

#include <list>

class PageContentContext;

typedef std::list<ObjectIDType> ObjectIDTypeList;

class PDFPage
{
public:
	PDFPage(void);
	~PDFPage(void);

	void SetMediaBox(const PDFRectangle& inMediaBox);
	const PDFRectangle& GetMediaBox() const;

	ResourcesDictionary& GetResourcesDictionary();

	void AddContentStreamReference(ObjectIDType inStreamReference);
	ObjectIDType GetContentStreamsCount();
	SingleValueContainerIterator<ObjectIDTypeList> GetContentStreamReferencesIterator();

	// extensibility, direct access to the content context used for this page..
	PageContentContext* GetAssociatedContentContext();
	void DisassociateContentContext();
	void AssociateContentContext(PageContentContext* inContentContext);

private:
	PDFRectangle mMediaBox;
	ObjectIDTypeList mContentStreamReferences;
	ResourcesDictionary mResources;
	PageContentContext* mContentContext;

};

/*
   Source File : PDFPage.cpp


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
#include "PDFPage.h"

PDFPage::PDFPage(void)
{
}

PDFPage::~PDFPage(void)
{
}


void PDFPage::SetMediaBox(const PDFRectangle& inMediaBox)
{
	mMediaBox = inMediaBox;
}

const PDFRectangle& PDFPage::GetMediaBox() const
{
	return mMediaBox;
}

void PDFPage::AddContentStreamReference(ObjectIDType inStreamReference)
{
	mContentStreamReferences.push_back(inStreamReference);
}

ObjectIDType PDFPage::GetContentStreamsCount()
{
	return (ObjectIDType)mContentStreamReferences.size();
}

SingleValueContainerIterator<ObjectIDTypeList> PDFPage::GetContentStreamReferencesIterator()
{
	return SingleValueContainerIterator<ObjectIDTypeList>(mContentStreamReferences);
}

ResourcesDictionary& PDFPage::GetResourcesDictionary()
{
	return mResources;
}

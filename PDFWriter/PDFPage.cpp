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

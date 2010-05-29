#pragma once

#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"
#include "SingleValueContainerIterator.h"
#include "ResourcesDictionary.h"

#include <list>

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

private:
	PDFRectangle mMediaBox;
	ObjectIDTypeList mContentStreamReferences;
	ResourcesDictionary mResources;

};

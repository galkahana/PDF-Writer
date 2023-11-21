#pragma once

#include <string>
#include <list>

#include "ObjectsBasicTypes.h"
#include "DocumentContextExtenderAdapter.h"

class AnnotationsWriter : public DocumentContextExtenderAdapter
{
public:

	AnnotationsWriter();

	EStatusCode OnPageWrite(
						PDFPage* inPage,
						DictionaryContext* inPageDictionaryContext,
						ObjectsContext* inPDFWriterObjectContext,
						DocumentContext* inPDFWriterDocumentContext);	
	void SetAnnotationObjectReference(ObjectIDType inAnnotationObjectRef);

private:

	ObjectIDType mAnnotationObjectRef;
};
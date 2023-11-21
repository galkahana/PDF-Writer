#include "AnnotationsWriter.h"
#include "DictionaryContext.h"

#include "Trace.h"

AnnotationsWriter::AnnotationsWriter()
{
	mAnnotationObjectRef = 0;
}

void AnnotationsWriter::SetAnnotationObjectReference(ObjectIDType inAnnotationObjectRef)
{
	mAnnotationObjectRef = inAnnotationObjectRef;
}

EStatusCode AnnotationsWriter::OnPageWrite(
									PDFPage* inPage,
									DictionaryContext* inPageDictionaryContext,
									ObjectsContext* inPDFWriterObjectContext,
									DocumentContext* inPDFWriterDocumentContext)
{
	// write the comments as the page array of annotations
	
	if(mAnnotationObjectRef == 0)
		return eSuccess;

	if(inPageDictionaryContext->WriteKey("Annots") != eSuccess)
	{
		// Failed to write Annots key! there's already an annots entry. hmm. need to coordinate with another option
		TRACE_LOG("AnnotationsWriter::OnPageWrite, Exception. Annots already written for this page, can't write a new entr");
		return eFailure;
	}

	inPageDictionaryContext->WriteNewObjectReferenceValue(mAnnotationObjectRef);


	// reset for next time
	mAnnotationObjectRef = 0;
	return eSuccess;	
}


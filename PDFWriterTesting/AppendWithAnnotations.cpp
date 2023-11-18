// standard library includes
#include <iostream>
#include <string>
#include <list>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"
#include "RefCountPtr.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFIndirectObjectReference.h"
#include "PDFArray.h"
#include "DocumentContext.h"
#include "DocumentContextExtenderAdapter.h"
#include "DictionaryContext.h"
#include "Trace.h"


#include "testing/TestIO.h"

typedef list<ObjectIDType> ObjectIDTypeList;

class AnnotationsWriter : public DocumentContextExtenderAdapter
{
public:

	AnnotationsWriter();

	EStatusCode OnPageWrite(
						PDFPage* inPage,
						DictionaryContext* inPageDictionaryContext,
						ObjectsContext* inPDFWriterObjectContext,
						DocumentContext* inPDFWriterDocumentContext);	
	void AddCopiedAnnotation(ObjectIDType inNewAnnotation);

private:

	ObjectIDTypeList mAnnotationsIDs;
};

AnnotationsWriter::AnnotationsWriter()
{
}

void AnnotationsWriter::AddCopiedAnnotation(ObjectIDType inNewAnnotation)
{
	mAnnotationsIDs.push_back(inNewAnnotation);
}

EStatusCode AnnotationsWriter::OnPageWrite(
									PDFPage* inPage,
									DictionaryContext* inPageDictionaryContext,
									ObjectsContext* inPDFWriterObjectContext,
									DocumentContext* inPDFWriterDocumentContext)
{
	// write the comments as the page array of annotations
	
	if(mAnnotationsIDs.size() == 0)
		return eSuccess;

	if(inPageDictionaryContext->WriteKey("Annots") != eSuccess)
	{
		// Failed to write Annots key! there's already an annots entry. hmm. need to coordinate with another option
		TRACE_LOG("AnnotationsWriter::OnPageWrite, Exception. Annots already written for this page, can't write a new entr");
		return eFailure;
	}

	ObjectIDTypeList::iterator it = mAnnotationsIDs.begin();

	inPDFWriterObjectContext->StartArray();
	for(; it != mAnnotationsIDs.end(); ++it)
		inPDFWriterObjectContext->WriteIndirectObjectReference(*it);
	inPDFWriterObjectContext->EndArray(eTokenSeparatorEndLine);
	
	mAnnotationsIDs.clear();
	return eSuccess;	
}

EStatusCode EmbedPagesInPDF(PDFWriter* inTargetWriter, const string& inSourcePDF)
{
	EStatusCode status = eSuccess;
	PDFDocumentCopyingContext* copyingContext;
	AnnotationsWriter annotationsWriter;
	
	inTargetWriter->GetDocumentContext().AddDocumentContextExtender(&annotationsWriter);
	do
	{

		copyingContext = inTargetWriter->CreatePDFCopyingContext(inSourcePDF);
		if(!copyingContext)
		{
			status = eFailure;
			break;
		}

		unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
		for(unsigned long i=0; i < pagesCount && eSuccess == status; ++i)
		{
			RefCountPtr<PDFDictionary> pageDictionary(copyingContext->GetSourceDocumentParser()->ParsePage(i));			

			PDFObjectCastPtr<PDFArray> annotations(copyingContext->GetSourceDocumentParser()->QueryDictionaryObject(pageDictionary.GetPtr(),"Annots"));
			if(annotations.GetPtr())
			{
				SingleValueContainerIterator<PDFObjectVector> annotationDictionaryObjects =  annotations->GetIterator();

				PDFObjectCastPtr<PDFIndirectObjectReference> annotationReference;
				while(annotationDictionaryObjects.MoveNext() && eSuccess == status)
				{
					annotationReference = annotationDictionaryObjects.GetItem();
					EStatusCodeAndObjectIDType result = copyingContext->CopyObject(annotationReference->mObjectID);
					status = result.first;
					if(eSuccess == status)
						annotationsWriter.AddCopiedAnnotation(result.second);
				}
				if(status != eSuccess)
					break;
			}

			status = copyingContext->AppendPDFPageFromPDF(i).first;


		}
	} while(false);

	delete copyingContext;
	inTargetWriter->GetDocumentContext().RemoveDocumentContextExtender(&annotationsWriter);
	return status;
}


int AppendWithAnnotations(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv, "CopyingPagesWithComments.pdf"),ePDFVersion13);
		if(status != eSuccess)
			break;

		// last but not least
		status = EmbedPagesInPDF(&pdfWriter, BuildRelativeInputPath(argv,"a1.pdf"));
		if(status != eSuccess)
			break;


		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;		

	}while(false);

	return status == eSuccess ? 0:1;
}
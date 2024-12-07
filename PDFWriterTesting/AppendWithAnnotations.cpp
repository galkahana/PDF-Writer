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
#include "AnnotationsWriter.h"

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

			if(pageDictionary->Exists("Annots")) {
				// just copy the annots object into an indirect object, if it's not already that...later we can just attach the ref to the page.
				// copying will take care of recreating the annots object and internal object in whatever way there are in the original document,
				// so no need ot worry about whether they are references or direct objects
				ObjectIDType annotsRef;
				RefCountPtr<PDFObject> annotsObject = pageDictionary->QueryDirectObject("Annots");

				if(annotsObject->GetType() == PDFObject::ePDFObjectIndirectObjectReference) {
					annotsRef = ((PDFIndirectObjectReference*)(annotsObject.GetPtr()))->mObjectID;
				} else {
					// direct object. recreate as indirect, for the sake of simplicity. after its done copying, copy what indirect objects
					// it refers to (indirect annotation objects...normally)
					ObjectsContext& objectsContext = inTargetWriter->GetObjectsContext();

					annotsRef = objectsContext.StartNewIndirectObject();	
					if(annotsRef == 0) {
						status = eFailure;
						break;
					}
					EStatusCodeAndObjectIDTypeList result = copyingContext->CopyDirectObjectWithDeepCopy(annotsObject.GetPtr());

					status = result.first;
					if(status != eSuccess)
					 	break;

					objectsContext.EndIndirectObject();

					// done copying the annots object, now copy its depenencies
					status = copyingContext->CopyNewObjectsForDirectObject(result.second);
					if(status != eSuccess)
						break;
				}

				// report to end page handler, so it can use this reference when writing the page
				annotationsWriter.SetAnnotationObjectReference(annotsRef);

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

		// comment annotations
		status = EmbedPagesInPDF(&pdfWriter, BuildRelativeInputPath(argv,"MakingComments.pdf"));
		if(status != eSuccess)
			break;


		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;		

	}while(false);

	return status == eSuccess ? 0:1;
}
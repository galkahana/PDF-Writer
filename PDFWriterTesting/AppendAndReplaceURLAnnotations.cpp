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
#include "ParsedPrimitiveHelper.h"
#include "Trace.h"


#include "testing/TestIO.h"

#include "AnnotationsWriter.h"


typedef list<ObjectIDType> ObjectIDTypeList;


bool ShouldAnnotationBeReplaced(PDFParser* inParser, PDFDictionary* inAnnotation) {
	// this is some sample logic for deciding if to modify an annotation.
	// here it's determined by whether the uri is pdfhummus.com

    RefCountPtr<PDFObject> subType = inParser->QueryDictionaryObject(inAnnotation,"Subtype");

	// make sure the annotation is a link
	if(ParsedPrimitiveHelper(subType.GetPtr()).ToString() != "Link")
		return false;

	// grab the action dictionary and find a URI key
	PDFObjectCastPtr<PDFDictionary> action = inParser->QueryDictionaryObject(inAnnotation,"A");
	if(!action)
		return false;

    RefCountPtr<PDFObject> uri = inParser->QueryDictionaryObject(action.GetPtr(),"URI");

	string uriString = ParsedPrimitiveHelper(uri.GetPtr()).ToString();

	// now we can check the uri is something that we want to replace
	return "http://www.pdfhummus.com" == uriString;
}

EStatusCodeAndObjectIDType ReplaceAnnotation(ObjectsContext& objectsContext, PDFDocumentCopyingContext* inCopyingContext, PDFParser* inParser, PDFDictionary* inAnnotation) {
	// this is samples replacement logic, where here the url is replaced to github.com (and since it's 2023 it's https)

	// we'll do this in 2 stages:
	// copy all but action dict
	// in action dict copy all but uri entry which we'll replace
	EStatusCode status = eSuccess;
	ObjectIDTypeList referencedObjects;
	ObjectIDType newAnnotationRef = objectsContext.StartNewIndirectObject();

	if(newAnnotationRef == 0)
		return EStatusCodeAndObjectIDType(eFailure,0);

	do {

		DictionaryContext* newAnnotationDicitonary = objectsContext.StartDictionary();
		MapIterator<PDFNameToPDFObjectMap>  it = inAnnotation->GetIterator();

		// copy all but action dict
        while(it.MoveNext() && eSuccess == status)
        {
			if(it.GetKey()->GetValue() == "A")
				continue;
            
			newAnnotationDicitonary->WriteKey(it.GetKey()->GetValue());
			EStatusCodeAndObjectIDTypeList result = inCopyingContext->CopyDirectObjectWithDeepCopy(it.GetValue());
			status = result.first;
			referencedObjects.insert(referencedObjects.end(), result.second.begin(), result.second.end());
        } 
		if(status != eSuccess)
			break;

		// create a new action dict
		newAnnotationDicitonary->WriteKey("A");
		DictionaryContext* newActionDictionary = objectsContext.StartDictionary();

		PDFObjectCastPtr<PDFDictionary> action = inParser->QueryDictionaryObject(inAnnotation,"A");

		it = action->GetIterator();

		// copy all but uri string
        while(it.MoveNext() && eSuccess == status)
        {
			if(it.GetKey()->GetValue() == "URI")
				continue;
            
			newActionDictionary->WriteKey(it.GetKey()->GetValue());
			EStatusCodeAndObjectIDTypeList result = inCopyingContext->CopyDirectObjectWithDeepCopy(it.GetValue());
			status = result.first;
			referencedObjects.insert(referencedObjects.end(), result.second.begin(), result.second.end());
        } 
		if(status != eSuccess)
			break;

		// write new uri (FINALLLYY)
		newActionDictionary->WriteKey("URI");
		newActionDictionary->WriteLiteralStringValue("https://www.github.com");

		objectsContext.EndDictionary(newActionDictionary);
		objectsContext.EndDictionary(newAnnotationDicitonary);
		objectsContext.EndIndirectObject();

		status = inCopyingContext->CopyNewObjectsForDirectObject(referencedObjects);
	}while(false);

	return EStatusCodeAndObjectIDType(status,newAnnotationRef);
}

EStatusCode EmbedPagesInPDFAndReplaceUrl(PDFWriter* inTargetWriter, const string& inSourcePDF)
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

		PDFParser* parser = copyingContext->GetSourceDocumentParser();

		unsigned long pagesCount = parser->GetPagesCount();
		for(unsigned long i=0; i < pagesCount && eSuccess == status; ++i)
		{
			RefCountPtr<PDFDictionary> pageDictionary(parser->ParsePage(i));
			// In this example we'll copy the annotations one by one so we can inspect them and determine
			// if they are a link url for a url we wish to replace
			
			PDFObjectCastPtr<PDFArray> annotations = parser->QueryDictionaryObject(pageDictionary.GetPtr(), "Annots");


			if(!!annotations) {
				// the plan is this:
				// iterate the annotations object. 
				// for each annotation we decide if we want to replace it or not.
				// if we dont want to replace, append it to a list as is
				// if we do want to replace it, create a replacement annotation and add to the list a reference to it
				// (for the sake of simplicity a replaced object will always be a referenced one)

				
				// where we'll store ids for annotations that got replaced
				ObjectIDTypeList replacedAnnotations;
				ObjectsContext& objectsContext = inTargetWriter->GetObjectsContext();

				for(unsigned long i = 0; i < annotations->GetLength() && eSuccess == status; ++i) {
					PDFObjectCastPtr<PDFDictionary> annotation = parser->QueryArrayObject(annotations.GetPtr(),i);
					if(ShouldAnnotationBeReplaced(parser, annotation.GetPtr())) {
						EStatusCodeAndObjectIDType result = ReplaceAnnotation(objectsContext, copyingContext, parser, annotation.GetPtr());
						status = result.first;
						replacedAnnotations.push_back(result.second);
					}
					else {
						replacedAnnotations.push_back(0); // mark this annotations to be copied as is
					}
				}
				if(status != eSuccess)
					break;


				// finally recreate the annotations array by copying either an original annotation or a new one,
				// depending on which got replaced
				ObjectIDType annotsRef;

				annotsRef = objectsContext.StartNewIndirectObject();	
				if(annotsRef == 0) {
					status = eFailure;
					break;
				}
				objectsContext.StartArray();

				ObjectIDTypeList referencedObjects;
				ObjectIDTypeList::iterator it = replacedAnnotations.begin(); // note that it and i should be synched
				for(unsigned long i = 0; i < annotations->GetLength() && eSuccess == status; ++i, ++it) {
					if(0 == *it) {
						// didnt change, copy as is
						RefCountPtr<PDFObject> annotation = annotations->QueryObject(i);
						EStatusCodeAndObjectIDTypeList result = copyingContext->CopyDirectObjectWithDeepCopy(annotation.GetPtr());
						status = result.first;
						// accumulate (appends) indirect objects referenced by annotation that we'll copy in the end
						referencedObjects.insert(referencedObjects.end(),result.second.begin(),result.second.end());
					}
					else {
						// write indirect reference
						objectsContext.WriteNewIndirectObjectReference(*it);
					}
				}
				if(status != eSuccess)
					break;

				objectsContext.EndArray();
				objectsContext.EndIndirectObject();

				// copy all accumulated referenced objects
				status = copyingContext->CopyNewObjectsForDirectObject(referencedObjects);


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


int AppendAndReplaceURLAnnotations(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv, "CopyingPagesWithLinks.pdf"),ePDFVersion13);
		if(status != eSuccess)
			break;

		// link annotations (in this example we'll go a bit deeper and replace a link while copying)
		// end result replaced pdfhummus.com with github.com
		status = EmbedPagesInPDFAndReplaceUrl(&pdfWriter, BuildRelativeInputPath(argv,"LinksTest.pdf"));
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;		

	}while(false);

	return status == eSuccess ? 0:1;
}
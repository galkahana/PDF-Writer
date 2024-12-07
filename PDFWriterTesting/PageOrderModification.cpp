/*
Source File : PageOrderModification.cpp


Copyright 2013 Gal Kahana PDFWriter

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
#include "PDFWriter.h"
#include "DocumentContext.h"
#include "CatalogInformation.h"
#include "PDFObjectCast.h"
#include "PDFIndirectObjectReference.h"
#include "PDFDocumentCopyingContext.h"
#include "DictionaryContext.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace PDFHummus;

typedef std::vector<ObjectIDType> ULongVector;

static EStatusCode AddPageToNewTree(unsigned long inPageIndex, PDFWriter& inWriter, PDFDocumentCopyingContext* inCopyingContext) {
	CatalogInformation& catalogInformation = inWriter.GetDocumentContext().GetCatalogInformation();
	IndirectObjectsReferenceRegistry& objectsRegistry = inWriter.GetObjectsContext().GetInDirectObjectsRegistry();
	PDFParser& modifiedFileParser = inWriter.GetModifiedFileParser();

	ObjectIDType pageObjectId = modifiedFileParser.GetPageObjectID(inPageIndex);

	// re-add the page to the new page tree
	ObjectIDType newParent =  catalogInformation.AddPageToPageTree(pageObjectId, objectsRegistry);

	// now modify the page object to refer to the new parent
	PDFObjectCastPtr<PDFDictionary> pageDictionaryObject = modifiedFileParser.ParsePage(inPageIndex);
	MapIterator<PDFNameToPDFObjectMap>  pageDictionaryObjectIt = pageDictionaryObject->GetIterator();
	ObjectsContext& objectContext = inWriter.GetObjectsContext();


	EStatusCode status = objectContext.StartModifiedIndirectObject(pageObjectId);
	if(status != eSuccess) {
		cout<<"failed to start modified page object\n";
		return status;
	}
	DictionaryContext* modifiedPageObject = objectContext.StartDictionary();

	// copy all elements of the page to the new page object, but the "Parent" element
	while (pageDictionaryObjectIt.MoveNext())
	{
		if (pageDictionaryObjectIt.GetKey()->GetValue() != "Parent")
		{
			modifiedPageObject->WriteKey(pageDictionaryObjectIt.GetKey()->GetValue());
			inCopyingContext->CopyDirectObjectAsIs(pageDictionaryObjectIt.GetValue());
		}
	}

	// now add parent entry
	modifiedPageObject->WriteKey("Parent");
	modifiedPageObject->WriteNewObjectReferenceValue(newParent);

	objectContext.EndDictionary(modifiedPageObject);
	objectContext.EndIndirectObject();

	return status;

}

int PageOrderModification(int argc, char* argv[])
{

	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{

		// Modify existing page. first modify it's page box, then anoteher page content

		// open file for modification
		status = pdfWriter.ModifyPDF(
			BuildRelativeInputPath(argv, string("XObjectContent.pdf")),
			ePDFVersion13,
			BuildRelativeOutputPath(argv, string("XObjectContentOrderModified.pdf")),
			LogConfiguration(true, true, BuildRelativeOutputPath(argv, string("XObjectContentOrderModified.log"))));
		if (status != eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		PDFParser& modifiedFileParser = pdfWriter.GetModifiedFileParser();
		cout << "Document has " << modifiedFileParser.GetPagesCount() << " pages\n";
		for (unsigned long i = 0; i < modifiedFileParser.GetPagesCount(); ++i)
			cout << (i + 1) << ": is object id " << modifiedFileParser.GetPageObjectID(i) << "\n";
		
		// create a shared copying context to be used when re-addig the pages, to share elements
		PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContextForModifiedFile();

		// let's re-add the second page and then the first page, in this way changing their order. for this we'll need
		// direct access to the document context catalog information, which holds the page tree
		
		// add pages in any order that you want
		status = AddPageToNewTree(1, pdfWriter, copyingContext);
		if(status != eSuccess) 
		{
			cout << "failed to add page 1\n";
			break;
		}
		status = AddPageToNewTree(0, pdfWriter, copyingContext);
		if(status != eSuccess) 
		{
			cout << "failed to add page 0\n";
			break;
		}

		// delete previous page tree to allow override
		PDFObjectCastPtr<PDFIndirectObjectReference> catalogReference(modifiedFileParser.GetTrailer()->QueryDirectObject("Root"));
		PDFObjectCastPtr<PDFDictionary> catalog(modifiedFileParser.ParseNewObject(catalogReference->mObjectID));
		PDFObjectCastPtr<PDFIndirectObjectReference> pagesReference(catalog->QueryDirectObject("Pages"));
		IndirectObjectsReferenceRegistry& objectsRegistry = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry();

		status = objectsRegistry.DeleteObject(pagesReference->mObjectID);
		if(status != eSuccess) 
		{
			cout << "failed to delete previous page tree\n";
			break;
		}

		status = pdfWriter.EndPDF();
		if (status != eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}
	} while (false);

	return status == eSuccess ? 0:1;
}


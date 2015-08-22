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
#include "PageOrderModification.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "DocumentContext.h"
#include "CatalogInformation.h"
#include "PDFObjectCast.h"
#include "PDFIndirectObjectReference.h"


#include <iostream>

using namespace PDFHummus;

typedef std::vector<ObjectIDType> ULongVector;

PageOrderModification::PageOrderModification(void)
{
}


PageOrderModification::~PageOrderModification(void)
{
}


EStatusCode PageOrderModification::Run(const TestConfiguration& inTestConfiguration)
{

	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{

		// Modify existing page. first modify it's page box, then anoteher page content

		// open file for modification
		status = pdfWriter.ModifyPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, string("TestMaterials/XObjectContent.pdf")),
			ePDFVersion13,
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, string("XObjectContentOrderModified.pdf")),
			LogConfiguration(true, true, RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, string("XObjectContentOrderModified.log"))));
		if (status != eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		PDFParser& modifiedFileParser = pdfWriter.GetModifiedFileParser();
		cout << "Document has " << modifiedFileParser.GetPagesCount() << " pages\n";
		for (unsigned long i = 0; i < modifiedFileParser.GetPagesCount(); ++i)
			cout << (i + 1) << ": is object id " << modifiedFileParser.GetPageObjectID(i) << "\n";
		

		// let's re-add the second page and then the first page, in this way changing their order. for this we'll need
		// direct access to the document context catalog information, which holds the page tree
		CatalogInformation& catalogInformation = pdfWriter.GetDocumentContext().GetCatalogInformation();
		IndirectObjectsReferenceRegistry& objectsRegistry = pdfWriter.GetObjectsContext().GetInDirectObjectsRegistry();
		
		// add pages in any order that you want
		catalogInformation.AddPageToPageTree(modifiedFileParser.GetPageObjectID(1), objectsRegistry);
		catalogInformation.AddPageToPageTree(modifiedFileParser.GetPageObjectID(0), objectsRegistry);

		// delete previous page tree to allow override
		PDFObjectCastPtr<PDFIndirectObjectReference> catalogReference(modifiedFileParser.GetTrailer()->QueryDirectObject("Root"));
		PDFObjectCastPtr<PDFDictionary> catalog(modifiedFileParser.ParseNewObject(catalogReference->mObjectID));
		PDFObjectCastPtr<PDFIndirectObjectReference> pagesReference(catalog->QueryDirectObject("Pages"));
		objectsRegistry.DeleteObject(pagesReference->mObjectID);

		status = pdfWriter.EndPDF();
		if (status != eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}
	} while (false);

	return status;
}

ADD_CATEGORIZED_TEST(PageOrderModification, "Modification")
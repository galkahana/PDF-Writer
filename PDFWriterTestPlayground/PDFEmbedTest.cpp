/*
   Source File : PDFEmbedTest.cpp


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
#include "PDFEmbedTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PageContentContext.h"
#include "PDFPage.h"
#include "PDFFormXObject.h"

#include <iostream>

using namespace std;

PDFEmbedTest::PDFEmbedTest(void)
{
}

PDFEmbedTest::~PDFEmbedTest(void)
{
}


EStatusCode PDFEmbedTest::Run()
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\PDFEmbedTest.PDF",ePDFVersion13);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		// Create XObjects from PDF to embed
		EStatusCodeAndPDFFormXObjectList result = pdfWriter.CreateFormXObjectsFromPDF(L"C:\\PDFLibTests\\TestMaterials\\XObjectContent.PDF",PDFPageRange(),ePDFPageBoxMediaBox);
		if(result.first != eSuccess)
		{
			wcout<<"failed to create PDF XObjects from PDF file\n";
			status = result.first;
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);

		// place the first page in the top left corner of the document
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(result.second.front()->GetObjectID()));
		contentContext->Q();
		
		contentContext->G(0);
		contentContext->w(1);
		contentContext->re(0,421,297.5,421);
		contentContext->S();


		// place the second page in the bottom right corner of the document
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,297.5,0);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(result.second.back()->GetObjectID()));
		contentContext->Q();

		contentContext->G(0);
		contentContext->w(1);
		contentContext->re(297.5,0,297.5,421);
		contentContext->S();


		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		// delete all form xobjects now
		PDFFormXObjectList::iterator it = result.second.begin();
		for(; it != result.second.end();++it)
			delete (*it);
		result.second.clear();

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	return status;
}

ADD_CATEGORIZED_TEST(PDFEmbedTest,"PDFEmbedding")

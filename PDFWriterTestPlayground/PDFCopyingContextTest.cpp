/*
   Source File : PDFCopyingContextTest.cpp


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
#include "PDFCopyingContextTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

PDFCopyingContextTest::PDFCopyingContextTest(void)
{
}

PDFCopyingContextTest::~PDFCopyingContextTest(void)
{
}

EStatusCode PDFCopyingContextTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;
	PDFWriter pdfWriter;
	PDFDocumentCopyingContext* copyingContext = NULL;

	do
	{
		status = pdfWriter.StartPDF(
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"PDFCopyingContextTest.PDF"),ePDFVersion13,
                        LogConfiguration(true,true,
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"PDFCopyingContextTest.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	


		copyingContext = pdfWriter.CreatePDFCopyingContext(
                        RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/BasicTIFFImagesTest.PDF"));
		if(!copyingContext)
		{
			cout<<"failed to initialize copying context from BasicTIFFImagesTest\n";
			status = PDFHummus::eFailure;
			break;
		}

		EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(1);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append page 1 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		result = copyingContext->AppendPDFPageFromPDF(18);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append page 18 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		result = copyingContext->AppendPDFPageFromPDF(4);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append page 4 from BasicTIFFImagesTest.PDF\n";
			status = result.first;
			break;
		}

		copyingContext->End(); // delete will call End() as well...so can avoid

		delete copyingContext;
		copyingContext = NULL;

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	delete copyingContext;
	return status;

}

ADD_CATEGORIZED_TEST(PDFCopyingContextTest,"PDFEmbedding")
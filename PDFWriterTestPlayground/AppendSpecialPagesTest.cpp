/*
   Source File : AppendSpecialPagesTest.cpp


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
#include "AppendSpecialPagesTest.h"
#include "TestsRunner.h"
#include "PDFWriter.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

AppendSpecialPagesTest::AppendSpecialPagesTest(void)
{
}

AppendSpecialPagesTest::~AppendSpecialPagesTest(void)
{
}

EStatusCode AppendSpecialPagesTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{

		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"AppendSpecialPagesTest.PDF"),ePDFVersion13,LogConfiguration(true,true,
            RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"AppendSpecialPagesTestLog.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		EStatusCodeAndObjectIDTypeList result;

		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/Protected.pdf"),PDFPageRange());
		if(result.first == PDFHummus::eSuccess)
		{
			cout<<"failted to NOT ALLOW embedding of protected documents\n";
			status = PDFHummus::eFailure;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/ObjectStreamsModified.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from ObjectStreamsModified.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/ObjectStreams.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from ObjectStreams.pdf\n";
			status = result.first;
			break;
		}

		
		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/AddedItem.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from AddedItem.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/AddedPage.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from AddedPage.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/MultipleChange.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from MultipleChange.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/RemovedItem.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from RemovedItem.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/Linearized.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from RemovedItem.pdf\n";
			status = result.first;
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}

	}while(false);

	return status;
}

ADD_CATEGORIZED_TEST(AppendSpecialPagesTest,"PDFEmbedding")

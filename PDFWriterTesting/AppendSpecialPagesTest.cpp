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
#include "PDFWriter.h"

#include "testing/TestIO.h"


#include <iostream>

using namespace std;
using namespace PDFHummus;

int AppendSpecialPagesTest(int argc, char* argv[])
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{

		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"AppendSpecialPagesTest.pdf"),ePDFVersion13,LogConfiguration(true,true,
            BuildRelativeOutputPath(argv,"AppendSpecialPagesTestLog.txt")));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	

		EStatusCodeAndObjectIDTypeList result;

		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"Protected.pdf"),PDFPageRange());
		if(result.first == PDFHummus::eSuccess)
		{
			cout<<"failted to NOT ALLOW embedding of protected documents without providing pasword\n";
			status = PDFHummus::eFailure;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"ObjectStreamsModified.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from ObjectStreamsModified.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"ObjectStreams.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from ObjectStreams.pdf\n";
			status = result.first;
			break;
		}

		
		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"AddedItem.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from AddedItem.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"AddedPage.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from AddedPage.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"MultipleChange.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from MultipleChange.pdf\n";
			status = result.first;
			break;
		}

		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"RemovedItem.pdf"),PDFPageRange());
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append pages from RemovedItem.pdf\n";
			status = result.first;
			break;
		}


		result = pdfWriter.AppendPDFPagesFromPDF(BuildRelativeInputPath(argv,"Linearized.pdf"),PDFPageRange());
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

	return status == eSuccess ? 0:1;
}
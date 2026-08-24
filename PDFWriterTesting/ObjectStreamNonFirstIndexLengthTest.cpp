/*
   Source File : ObjectStreamNonFirstIndexLengthTest.cpp


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

// Regression test for a bug in PDFParser::ParseExistingInDirectStreamObject: resolving an
// indirect /Length that lives at a non-first index of a compressed object stream (PDF 1.5+
// /ObjStm) could silently misread the wrong bytes, causing the page merge to fail with
// "unable to append to page, make sure source file exists". The fixture is a minimal,
// hand-built 2-page PDF (no external tools involved in producing it) where page 1's content
// stream's /Length is the first object packed into a shared ObjStm and page 2's /Length is
// the second -- reproducing the failure without depending on any particular PDF producer.
//
// The bug only surfaces on the *first* access to the shared object stream, so each page is
// appended through its own fresh PDFWriter/PDFParser below -- appending page 0 first would
// populate the parser's per-stream header cache at revision 0 and mask the bug when page 1
// is requested afterwards from the same parser.
static EStatusCode AppendSinglePage(int argc, char* argv[], unsigned long inPageIndex, const char* inOutputFileName)
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,inOutputFileName),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF "<<inOutputFileName<<"\n";
			break;
		}

		PDFPageRange range;
		range.mType = PDFPageRange::eRangeTypeSpecific;
		range.mSpecificRanges.push_back(ULongAndULong(inPageIndex,inPageIndex));

		EStatusCodeAndObjectIDTypeList result = pdfWriter.AppendPDFPagesFromPDF(
			BuildRelativeInputPath(argv,"ObjectStreamNonFirstIndexLength.pdf"),range);
		if(result.first != PDFHummus::eSuccess)
		{
			cout<<"failed to append page "<<inPageIndex<<" from ObjectStreamNonFirstIndexLength.pdf\n";
			status = result.first;
			break;
		}
		if(result.second.size() != 1)
		{
			cout<<"expecting 1 appended page, got "<<result.second.size()<<"\n";
			status = PDFHummus::eFailure;
			break;
		}

		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF for "<<inOutputFileName<<"\n";
			break;
		}

	}while(false);

	return status;
}

int ObjectStreamNonFirstIndexLengthTest(int argc, char* argv[])
{
	EStatusCode status;

	do
	{
		status = AppendSinglePage(argc,argv,1,"ObjectStreamNonFirstIndexLengthTest_page1.pdf");
		if(status != PDFHummus::eSuccess)
			break;

		status = AppendSinglePage(argc,argv,0,"ObjectStreamNonFirstIndexLengthTest_page0.pdf");
		if(status != PDFHummus::eSuccess)
			break;

	}while(false);

	return status == eSuccess ? 0:1;
}

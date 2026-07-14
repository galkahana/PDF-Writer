/*
   Source File : DoubleEndPDFTest.cpp


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
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "OutputStringBufferStream.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

// Test 1: Double EndPDF on file-based PDFWriter
// Without fix: SEGFAULT
// With fix: Second call returns eFailure gracefully
static int TestDoubleEndPDFFile(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	cout<<"Test 1: Double EndPDF on file output\n";

	status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"DoubleEndPDFTest.pdf"),ePDFVersion14);
	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not start PDF\n";
		return 1;
	}

	PDFPage* page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0,0,595,842));
	status = pdfWriter.WritePage(page);
	delete page;

	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not write page\n";
		return 1;
	}

	// First EndPDF - should succeed
	cout<<"  Calling EndPDF first time...\n";
	status = pdfWriter.EndPDF();
	if(status != eSuccess)
	{
		cout<<"  FAIL: First EndPDF failed (unexpected)\n";
		return 1;
	}
	cout<<"  First EndPDF succeeded\n";

	// Second EndPDF - should return failure (not crash!)
	cout<<"  Calling EndPDF second time...\n";
	status = pdfWriter.EndPDF();

	// After fix: second call should return eFailure gracefully
	if(status != eFailure)
	{
		cout<<"  FAIL: Second EndPDF should return eFailure\n";
		return 1;
	}
	cout<<"  Second EndPDF returned eFailure (correct behavior)\n";

	cout<<"  PASS\n";
	return 0;
}

// Test 2: Double EndPDFForStream on stream-based PDFWriter
// Without fix: SEGFAULT
// With fix: Second call returns eFailure gracefully
static int TestDoubleEndPDFForStream(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;
	OutputStringBufferStream outputStream;

	cout<<"Test 2: Double EndPDFForStream on stream output\n";

	status = pdfWriter.StartPDFForStream(&outputStream,ePDFVersion14);
	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not start PDF for stream\n";
		return 1;
	}

	PDFPage* page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0,0,595,842));
	status = pdfWriter.WritePage(page);
	delete page;

	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not write page\n";
		return 1;
	}

	// First EndPDFForStream - should succeed
	cout<<"  Calling EndPDFForStream first time...\n";
	status = pdfWriter.EndPDFForStream();
	if(status != eSuccess)
	{
		cout<<"  FAIL: First EndPDFForStream failed (unexpected)\n";
		return 1;
	}
	cout<<"  First EndPDFForStream succeeded\n";

	// Second EndPDFForStream - should return failure (not crash!)
	cout<<"  Calling EndPDFForStream second time...\n";
	status = pdfWriter.EndPDFForStream();

	if(status != eFailure)
	{
		cout<<"  FAIL: Second EndPDFForStream should return eFailure\n";
		return 1;
	}
	cout<<"  Second EndPDFForStream returned eFailure (correct behavior)\n";

	cout<<"  PASS\n";
	return 0;
}

// Test 3: Verify reuse after Reset works correctly
static int TestReuseAfterReset(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	cout<<"Test 3: Reuse PDFWriter after Reset\n";

	// First PDF
	status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"ReuseAfterReset1.pdf"),ePDFVersion14);
	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not start first PDF\n";
		return 1;
	}

	PDFPage* page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0,0,595,842));
	pdfWriter.WritePage(page);
	delete page;

	// Use Reset instead of EndPDF (aborts the PDF)
	cout<<"  Calling Reset on first PDF...\n";
	pdfWriter.Reset();
	cout<<"  Reset completed\n";

	// Second PDF - should work after Reset
	cout<<"  Starting second PDF after Reset...\n";
	status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"ReuseAfterReset2.pdf"),ePDFVersion14);
	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not start second PDF after Reset\n";
		return 1;
	}

	page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0,0,595,842));
	pdfWriter.WritePage(page);
	delete page;

	status = pdfWriter.EndPDF();
	if(status != eSuccess)
	{
		cout<<"  FAIL: Could not end second PDF\n";
		return 1;
	}
	cout<<"  Second PDF completed successfully\n";

	cout<<"  PASS\n";
	return 0;
}

// Test 4: EndPDF without StartPDF should fail gracefully
static int TestEndPDFWithoutStart(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	cout<<"Test 4: EndPDF without StartPDF\n";

	// Call EndPDF without ever calling StartPDF
	cout<<"  Calling EndPDF without StartPDF...\n";
	status = pdfWriter.EndPDF();

	if(status != eFailure)
	{
		cout<<"  FAIL: EndPDF without StartPDF should return eFailure\n";
		return 1;
	}
	cout<<"  EndPDF returned eFailure (correct behavior)\n";

	cout<<"  PASS\n";
	return 0;
}

// Main test function
int DoubleEndPDFTest(int argc, char* argv[])
{
	int result = 0;

	cout<<"=============================================================\n";
	cout<<"DoubleEndPDFTest: Testing PDFWriter double EndPDF protection\n";
	cout<<"=============================================================\n";
	cout<<"\n";

	result |= TestDoubleEndPDFFile(argc,argv);
	cout<<"\n";

	result |= TestDoubleEndPDFForStream(argc,argv);
	cout<<"\n";

	result |= TestReuseAfterReset(argc,argv);
	cout<<"\n";

	result |= TestEndPDFWithoutStart(argc,argv);
	cout<<"\n";

	cout<<"=============================================================\n";
	if(result == 0)
		cout<<"All tests PASSED\n";
	else
		cout<<"Some tests FAILED\n";
	cout<<"=============================================================\n";

	return result;
}

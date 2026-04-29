/*
   Source File : ParentCycleSanity.cpp


   Copyright 2026 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   Regression test for the /Parent cycle / unbounded recursion issues fixed in
   PRs #346, #347, #348 and refactored onto PDFParser::QueryInheritedDictionaryEntry.

   ParentCycle.pdf contains a single page whose /Parent points back to itself
   and which has no /Resources. Walking the parent chain looking for inherited
   /Resources used to recurse forever -> stack overflow. With the fix in place
   each consumer-layer caller must return with success, as a simple pdf with one empty page.
*/
#include "PDFParser.h"
#include "PDFPageInput.h"
#include "PDFModifiedPage.h"
#include "PDFWriter.h"
#include "PDFEmbedParameterTypes.h"
#include "InputFile.h"
#include "EStatusCode.h"
#include "Trace.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

static EStatusCode exercisePageInputPath(const string& inSourcePath) {
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(inSourcePath) != eSuccess)
		return eFailure;
	if(parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess)
		return eFailure;

	PDFPageInput pageInput(&parser, parser.ParsePage(0));
	PDFRectangle mediaBox = pageInput.GetMediaBox();
	if(mediaBox.GetWidth() != 612 || mediaBox.GetHeight() != 792) {
		cout << "ParentCycleSanity: PDFPageInput path failed to get correct media box" << endl;
		return eFailure;
	}
	return eSuccess;
}

static EStatusCode exerciseModifiedPagePath(const string& inSourcePath, const string& inOutputPath) {
	PDFWriter writer;
	EStatusCode status;
	status = writer.ModifyPDF(inSourcePath, ePDFVersion14, inOutputPath);
	if(status != eSuccess)
		return eFailure;

	PDFModifiedPage modifiedPage(&writer, 0);
	AbstractContentContext* context = modifiedPage.StartContentContext();
	if(!context)
		return eFailure;
	status = modifiedPage.EndContentContext();
	if(status != eSuccess)
		return eFailure;
	status = modifiedPage.WritePage();
	if(status != eSuccess)
		return eFailure;

	return writer.EndPDF();
}

static EStatusCode exerciseDocumentHandlerPath(const string& inSourcePath, const string& inOutputPath) {
	PDFWriter writer;
	EStatusCode status;
	status = writer.StartPDF(inOutputPath, ePDFVersion14);
	if(status != eSuccess)
		return eFailure;

	// AppendPDFPagesFromPDF -> FindPageResources walks the /Parent chain. if can't handle cycles, will crash.
	// The append may shouldn't fail because it's expected to just be a simple empty page
	status = writer.AppendPDFPagesFromPDF(inSourcePath, PDFPageRange()).first;
	if(status != eSuccess)
		return eFailure;

	return writer.EndPDF();
}

int ParentCycleSanity(int argc, char* argv[]) {
	Trace::DefaultTrace().SetLogSettings(BuildRelativeOutputPath(argv, "ParentCycleSanityLog.txt"), true, true);

	const string sourcePath = BuildRelativeInputPath(argv, "ParentCycle.pdf");

	if(exercisePageInputPath(sourcePath) != eSuccess) {
		cout << "ParentCycleSanity: PDFPageInput path failed" << endl;
		return 1;
	}

	if(exerciseModifiedPagePath(sourcePath, BuildRelativeOutputPath(argv, "ParentCycle_Modified.pdf")) != eSuccess) {
		cout << "ParentCycleSanity: PDFModifiedPage path failed" << endl;
		return 1;
	}

	if(exerciseDocumentHandlerPath(sourcePath, BuildRelativeOutputPath(argv, "ParentCycle_Appended.pdf")) != eSuccess) {
		cout << "ParentCycleSanity: PDFDocumentHandler path failed" << endl;
		return 1;
	}

	return 0;
}

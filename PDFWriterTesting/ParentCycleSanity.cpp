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
   each consumer-layer caller must return (success or eFailure) without
   crashing the process.
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
	// These call QueryInheritedValue which walks /Parent. With the cycle
	// guard the call returns; without it the process stack-overflows.
	pageInput.GetMediaBox();
	pageInput.GetCropBox();
	pageInput.GetRotate();
	return eSuccess;
}

static EStatusCode exerciseModifiedPagePath(const string& inSourcePath, const string& inOutputPath) {
	PDFWriter writer;
	if(writer.ModifyPDF(inSourcePath, ePDFVersion14, inOutputPath) != eSuccess)
		return eFailure;

	PDFModifiedPage modifiedPage(&writer, 0);
	// StartContentContext -> findInheritedResources walks the /Parent chain.
	// May legitimately fail to start a context if the page has no resources;
	// we only require that it does not crash.
	modifiedPage.StartContentContext();
	modifiedPage.EndContentContext();
	modifiedPage.WritePage();

	writer.EndPDF();
	return eSuccess;
}

static EStatusCode exerciseDocumentHandlerPath(const string& inSourcePath, const string& inOutputPath) {
	PDFWriter writer;
	if(writer.StartPDF(inOutputPath, ePDFVersion14) != eSuccess)
		return eFailure;

	// AppendPDFPagesFromPDF -> FindPageResources walks the /Parent chain.
	// The append may itself fail because of the malformed source; the
	// regression check is just that we return.
	writer.AppendPDFPagesFromPDF(inSourcePath, PDFPageRange());

	writer.EndPDF();
	return eSuccess;
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

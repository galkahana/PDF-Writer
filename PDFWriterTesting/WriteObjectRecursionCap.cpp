/*
   Source File : WriteObjectRecursionCap.cpp


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


   Regression test for the PDFDocumentHandler::WriteObjectByType recursion
   depth cap. Parser-produced object trees are bounded by PDFObjectParser's
   own MAX_OBJECT_DEPTH (100), but the public copy API
   (PDFDocumentCopyingContext::CopyDirectObjectAsIs /
    CopyDirectObjectWithDeepCopy) accepts caller-supplied PDFObject* trees
   that can be constructed at arbitrary depth. Without the cap a deep
   synthetic tree would recurse through
   WriteObjectByType -> WriteArrayObject -> WriteObjectByType -> ...
   until the call stack is exhausted.

   This test asserts the cap fires at the right boundary: a tree exactly
   at MAX_WRITE_OBJECT_DEPTH (100 nested arrays) is accepted; one level
   beyond (101 nested arrays) is rejected with eFailure.
*/
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFArray.h"
#include "ObjectsContext.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

struct DepthCase {
	const char* label;
	int         depth;
	EStatusCode expected;
};

static const DepthCase scDepthCases[] = {
	{ "CopyDirectObjectAsIs_DepthAtCap_Succeeds",   100, eSuccess },
	{ "CopyDirectObjectAsIs_DepthOverCap_Fails",    101, eFailure },
};

static EStatusCode WriteEmptyOnePageSource(char* argv[], const string& inPath) {
	PDFWriter writer;
	if(writer.StartPDF(inPath, ePDFVersion14) != eSuccess)
		return eFailure;
	PDFPage* page = new PDFPage();
	page->SetMediaBox(PDFRectangle(0, 0, 100, 100));
	if(writer.WritePageAndRelease(page) != eSuccess)
		return eFailure;
	return writer.EndPDF();
}

// Build a chain of inDepth nested arrays: [[[...[]]]] with inDepth levels.
// Caller owns the returned root and must Release it.
static PDFArray* BuildNestedArrayChain(int inDepth) {
	PDFArray* root = new PDFArray();
	PDFArray* current = root;
	for(int d = 1; d < inDepth; ++d) {
		PDFArray* child = new PDFArray();
		current->AppendObject(child);
		child->Release(); // AppendObject AddRef'd, drop our reference
		current = child;
	}
	return root;
}

static EStatusCode RunDepthCases(char* argv[], const string& inSourcePath) {
	const size_t caseCount = sizeof(scDepthCases) / sizeof(scDepthCases[0]);
	for(size_t i = 0; i < caseCount; ++i) {
		const DepthCase& testCase = scDepthCases[i];
		const string outPath = BuildRelativeOutputPath(argv,
			string("WriteObjectRecursionCap_") + testCase.label + ".pdf");

		PDFWriter writer;

		// Arrange
		if(writer.StartPDF(outPath, ePDFVersion14) != eSuccess) {
			cout << "WriteObjectRecursionCap[" << testCase.label
			     << "]: StartPDF failed" << endl;
			return eFailure;
		}
		PDFDocumentCopyingContext* copyingContext =
			writer.CreatePDFCopyingContext(inSourcePath);
		if(!copyingContext) {
			cout << "WriteObjectRecursionCap[" << testCase.label
			     << "]: CreatePDFCopyingContext failed" << endl;
			return eFailure;
		}
		ObjectIDType wrapperID = writer.GetObjectsContext().StartNewIndirectObject();
		PDFArray* tree = BuildNestedArrayChain(testCase.depth);

		// Act
		EStatusCode status = copyingContext->CopyDirectObjectAsIs(tree);

		// Assert
		if(status != testCase.expected) {
			cout << "WriteObjectRecursionCap[" << testCase.label
			     << "]: CopyDirectObjectAsIs returned " << status
			     << ", expected " << testCase.expected << endl;
			tree->Release();
			delete copyingContext;
			return eFailure;
		}

		tree->Release();
		writer.GetObjectsContext().EndIndirectObject();
		delete copyingContext;
		// Over-cap case leaves the output in a partial state (StartArray was
		// emitted before the cap fired). Only require EndPDF success on the
		// at-cap path; the over-cap path is allowed to fail-close.
		EStatusCode endStatus = writer.EndPDF();
		if(testCase.expected == eSuccess && endStatus != eSuccess) {
			cout << "WriteObjectRecursionCap[" << testCase.label
			     << "]: EndPDF failed on the success path" << endl;
			return eFailure;
		}
		(void)wrapperID;
	}
	return eSuccess;
}

int WriteObjectRecursionCap(int argc, char* argv[]) {
	const string sourcePath =
		BuildRelativeOutputPath(argv, "WriteObjectRecursionCap_source.pdf");
	if(WriteEmptyOnePageSource(argv, sourcePath) != eSuccess) {
		cout << "WriteObjectRecursionCap: failed to write source PDF" << endl;
		return 1;
	}
	return RunDepthCases(argv, sourcePath) == eSuccess ? 0 : 1;
}

/*
   Source File : PDFDocumentHandlerTest.cpp


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
*/
#include "PDFWriter.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFArray.h"
#include "PDFInteger.h"
#include "ObjectsContext.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// Build a chain of inDepth nested arrays: [[[...[]]]] with inDepth levels.
// If inAddScalarLeaf is true, the innermost array holds a single PDFInteger
// instead of being empty (exercises the dispatcher path for a scalar that
// sits at the maximum container nesting — must not be rejected by the cap).
// Caller owns the returned root and must Release it.
static PDFArray* BuildNestedArrayChain(int inDepth, bool inAddScalarLeaf) {
	PDFArray* root = new PDFArray();
	PDFArray* current = root;
	for(int d = 1; d < inDepth; ++d) {
		PDFArray* child = new PDFArray();
		current->AppendObject(child);
		child->Release(); // AppendObject AddRef'd, drop our reference
		current = child;
	}
	if(inAddScalarLeaf) {
		PDFInteger* leaf = new PDFInteger(42);
		current->AppendObject(leaf);
		leaf->Release();
	}
	return root;
}

struct WriteObjectByTypeCase {
	const char* label;
	int         depth;
	bool        scalarLeaf;
	EStatusCode expected;
};

static const WriteObjectByTypeCase scWriteObjectByTypeCases[] = {
	// MAX_WRITE_OBJECT_DEPTH = 100 mirrors PDFObjectParser's MAX_OBJECT_DEPTH:
	// the cap counts container-nesting depth only (arrays/dicts/streams), so
	// scalars at the deepest container level must still be accepted. One
	// container level beyond the cap is rejected with eFailure.
	{ "DepthAtCapEmptyLeaf_Succeeds",   100, false, eSuccess },
	{ "DepthAtCapScalarLeaf_Succeeds",  100, true,  eSuccess },
	{ "DepthOverCap_Fails",             101, false, eFailure },
};

static bool RunWriteObjectByTypeCases(char* argv[], const string& inSourcePath) {
	const char* fnName = "WriteObjectByType";
	const size_t caseCount = sizeof(scWriteObjectByTypeCases) / sizeof(scWriteObjectByTypeCases[0]);
	for(size_t i = 0; i < caseCount; ++i) {
		const WriteObjectByTypeCase& testCase = scWriteObjectByTypeCases[i];

		// Arrange
		const string outPath = BuildRelativeOutputPath(argv,
			string("PDFDocumentHandlerTest_") + testCase.label + ".pdf");
		PDFWriter writer;
		if(writer.StartPDF(outPath, ePDFVersion14) != eSuccess) {
			cout << "PDFDocumentHandlerTest [" << fnName << "::" << testCase.label
			     << "]: StartPDF failed" << endl;
			return false;
		}
		PDFDocumentCopyingContext* copyingContext =
			writer.CreatePDFCopyingContext(inSourcePath);
		if(!copyingContext) {
			cout << "PDFDocumentHandlerTest [" << fnName << "::" << testCase.label
			     << "]: CreatePDFCopyingContext failed" << endl;
			return false;
		}
		writer.GetObjectsContext().StartNewIndirectObject();
		PDFArray* tree = BuildNestedArrayChain(testCase.depth, testCase.scalarLeaf);

		// Act
		EStatusCode status = copyingContext->CopyDirectObjectAsIs(tree);

		// Assert
		if(status != testCase.expected) {
			cout << "PDFDocumentHandlerTest [" << fnName << "::" << testCase.label
			     << "]: returned " << status << ", expected " << testCase.expected << endl;
			tree->Release();
			delete copyingContext;
			return false;
		}

		// Cleanup (best-effort; over-cap leaves the writer mid-array, so EndPDF
		// is only required to succeed on the at-cap path).
		tree->Release();
		writer.GetObjectsContext().EndIndirectObject();
		delete copyingContext;
		EStatusCode endStatus = writer.EndPDF();
		if(testCase.expected == eSuccess && endStatus != eSuccess) {
			cout << "PDFDocumentHandlerTest [" << fnName << "::" << testCase.label
			     << "]: EndPDF failed on the success path" << endl;
			return false;
		}
	}
	return true;
}

int PDFDocumentHandlerTest(int argc, char* argv[]) {
	const string sourcePath = BuildRelativeInputPath(argv, "EmptyOnePage.pdf");
	return RunWriteObjectByTypeCases(argv, sourcePath) ? 0 : 1;
}

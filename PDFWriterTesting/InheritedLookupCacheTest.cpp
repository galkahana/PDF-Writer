/*
   Source File : InheritedLookupCacheTest.cpp


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


   Test for PDFParser::QueryInheritedDictionaryEntry parsing each /Parent page tree node once.
   InheritedAttributes.pdf has three pages under one /Pages node, which holds /Resources and
   /MediaBox for all of them. Every lookup used to parse the /Pages node again (and with it
   its whole /Kids array), which made reading all pages of a flat page tree quadratic in the
   page count. Now the node is parsed once, so sibling pages get the very same inherited
   object, and the values are as before.
*/
#include "PDFParser.h"
#include "PDFPageInput.h"
#include "PDFDictionary.h"
#include "InputFile.h"
#include "EStatusCode.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

int InheritedLookupCacheTest(int argc, char* argv[]) {
	PDFParser parser;
	InputFile pdfFile;
	if(pdfFile.OpenFile(BuildRelativeInputPath(argv, "InheritedAttributes.pdf")) != eSuccess) {
		cout << "InheritedLookupCacheTest: failed to open InheritedAttributes.pdf" << endl;
		return 1;
	}
	if(parser.StartPDFParsing(pdfFile.GetInputStream()) != eSuccess) {
		cout << "InheritedLookupCacheTest: failed to start parsing InheritedAttributes.pdf" << endl;
		return 1;
	}
	if(parser.GetPagesCount() != 3) {
		cout << "InheritedLookupCacheTest: expected 3 pages, got " << parser.GetPagesCount() << endl;
		return 1;
	}

	RefCountPtr<PDFObject> firstResources;
	for(unsigned long i = 0; i < parser.GetPagesCount(); ++i) {
		RefCountPtr<PDFDictionary> page(parser.ParsePage(i));
		if(!page) {
			cout << "InheritedLookupCacheTest: failed to parse page " << i << endl;
			return 1;
		}

		RefCountPtr<PDFObject> resources(parser.QueryInheritedDictionaryEntry(page.GetPtr(), "Resources"));
		if(!resources || resources->GetType() != PDFObject::ePDFObjectDictionary) {
			cout << "InheritedLookupCacheTest: no inherited /Resources for page " << i << endl;
			return 1;
		}
		if(i == 0) {
			firstResources = resources;
		}
		else if(resources.GetPtr() != firstResources.GetPtr()) {
			cout << "InheritedLookupCacheTest: the /Pages node was parsed again for page " << i << endl;
			return 1;
		}

		PDFPageInput pageInput(&parser, page);
		PDFRectangle mediaBox = pageInput.GetMediaBox();
		if(mediaBox.GetWidth() != 612 || mediaBox.GetHeight() != 792) {
			cout << "InheritedLookupCacheTest: wrong inherited /MediaBox for page " << i << endl;
			return 1;
		}
	}

	return 0;
}

/*
   Source File : MergePDFPages.h


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
#pragma once
#include "ITestUnit.h"

class MergePDFPages : public ITestUnit
{
public:
	MergePDFPages(void);
	~MergePDFPages(void);

	virtual PDFHummus::EStatusCode Run();

private:

	PDFHummus::EStatusCode TestOnlyMerge();
	PDFHummus::EStatusCode TestPrefixGraphicsMerge();
	PDFHummus::EStatusCode TestSuffixGraphicsMerge();
	PDFHummus::EStatusCode TestBothGraphicsMerge();
	PDFHummus::EStatusCode MergeTwoPageInSeparatePhases();
	PDFHummus::EStatusCode MergeTwoPageWithEvents();
	PDFHummus::EStatusCode MergePagesUsingCopyingContext();

};

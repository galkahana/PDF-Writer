/*
 Source File : CopyingAndMergingEmptyPages.h
 
 
 Copyright 2012 Gal Kahana PDFWriter
 
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

class CopyingAndMergingEmptyPages : public ITestUnit
{
public:
	CopyingAndMergingEmptyPages(void);
	virtual ~CopyingAndMergingEmptyPages(void);
    
	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);
    
private:
    
    PDFHummus::EStatusCode PrepareSampleEmptyPDF(const TestConfiguration& inTestConfiguration, const string& inEmptyFileName);
    
    /*
     various tests:
        - create a form from an empty page and using
        - create a page from an empty page, and using
        - merging an empty page to an existing page
        - merging an empty page to a form
    */
    
    PDFHummus::EStatusCode CreateFormFromEmptyPage(const TestConfiguration& inTestConfiguration, const string& inEmptyFileName);
    PDFHummus::EStatusCode CreatePageFromEmptyPage(const TestConfiguration& inTestConfiguration, const string& inEmptyFileName);
    PDFHummus::EStatusCode MergeEmptyPageToPage(const TestConfiguration& inTestConfiguration, const string& inEmptyFileName);
    PDFHummus::EStatusCode MergeEmptyPageToForm(const TestConfiguration& inTestConfiguration, const string& inEmptyFileName);
};

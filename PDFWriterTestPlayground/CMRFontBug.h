/*
 Source File : CMRFontBugTest.h
 
 
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

#include <string>

class CFFFileInput;

class CMRFontBugTest : public ITestUnit
{
public:
	CMRFontBugTest(void);
	virtual ~CMRFontBugTest(void);
    
	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);

    
private:
    PDFHummus::EStatusCode CreatePDFFileWithFont(const TestConfiguration& inTestConfiguration,const std::string& inOutputFilePath);
    PDFHummus::EStatusCode InspectCreatedFontGlyphs(const TestConfiguration& inTestConfiguration,const std::string& inPDFFilePath);
    PDFHummus::EStatusCode SaveCharstringCode(const TestConfiguration& inTestConfiguration,unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput);
    

};


/*
   Source File : OpenTypeTest.h


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
#include "CFFFileInput.h"

class OpenTypeTest : public ITestUnit
{
public:
	OpenTypeTest(void);
	~OpenTypeTest(void);

	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);

private:
	PDFHummus::EStatusCode SaveCharstringCode(const TestConfiguration& inTestConfiguration,unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput);
	PDFHummus::EStatusCode TestFont(const TestConfiguration& inTestConfiguration);

};

/*
   Source File : PDFObjectCastTest.cpp


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
#include "PDFObjectCastTest.h"
#include "PDFObject.h"
#include "PDFName.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

PDFObjectCastTest::PDFObjectCastTest(void)
{
}

PDFObjectCastTest::~PDFObjectCastTest(void)
{
}

EStatusCode PDFObjectCastTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;


	PDFObjectCastPtr<PDFName> aNonName1(NULL);
	if(!(!aNonName1))
	{
		cout<<"Casting null to PDFName should provide NULL. fail\n";
		status = PDFHummus::eFailure;
	}

	PDFObjectCastPtr<PDFName> aNonName2(new PDFDictionary());
	if(!(!aNonName2))
	{
		cout<<"Casting PDFDictionary to PDFName should provide NULL. fail\n";
		status = PDFHummus::eFailure;
	}

	PDFObjectCastPtr<PDFName> aName3(new PDFName("aName"));
	if((!aName3))
	{
		cout<<"Casting PDName to PDFName should provide PDFName. fail\n";
		status = PDFHummus::eFailure;
	}

	if(aName3->GetValue() != "aName")
	{
		cout<<"should be aName, is "<<aName3->GetValue().c_str()<<"\n";
		status = PDFHummus::eFailure;
	}

	return status;
}

ADD_CATEGORIZED_TEST(PDFObjectCastTest,"PDFEmbedding")

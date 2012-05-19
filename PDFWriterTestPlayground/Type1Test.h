/*
   Source File : Type1Test.h


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

#include <string>
#include <vector>

struct Type1FontDictionary;
struct Type1PrivateDictionary;
struct Type1FontInfoDictionary;

using namespace std;

class Type1Input;

class Type1Test : public ITestUnit
{
public:
	Type1Test(void);
	~Type1Test(void);

	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);

private:
	PDFHummus::EStatusCode SaveCharstringCode(const TestConfiguration& inTestConfiguration,const string& inCharStringName,Type1Input* inType1Input);
	PDFHummus::EStatusCode ShowDependencies(const string& inCharStringName,Type1Input* inType1Input);

	void ShowFontDictionary(const Type1FontDictionary& inFontDictionary);
	void ShowFontInfoDictionary(const Type1FontInfoDictionary& inFontInfoDictionary);
	void ShowPrivateInfoDictionary(const Type1PrivateDictionary& inPrivateDictionary);
	void ShowDoubleArray(const double inDoubleArray[],int inSize);
	void ShowIntVector(const vector<int>& inVector);
	void ShowDoubleVector(const vector<double>& inVector);

};

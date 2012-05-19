/*
   Source File : InputFlateDecodeTester.cpp


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
#include "InputFlateDecodeTester.h"
#include "InputFlateDecodeStream.h"
#include "OutputFlateEncodeStream.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;
using namespace PDFHummus;

InputFlateDecodeTester::InputFlateDecodeTester(void)
{
}

InputFlateDecodeTester::~InputFlateDecodeTester(void)
{
}

EStatusCode InputFlateDecodeTester::Run(const TestConfiguration& inTestConfiguration)
{
	OutputFile outputFile;
	string aString("hello world");
	IOBasicTypes::Byte buffer;
	InputFlateDecodeStream inputDecoder;
	OutputFlateEncodeStream outputEncoder;

	outputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"source.txt"));
	outputEncoder.Assign(outputFile.GetOutputStream());
	outputEncoder.Write((IOBasicTypes::Byte*)aString.c_str(),aString.size());
	outputEncoder.Assign(NULL);
	outputFile.CloseFile();

	InputFile inputFile;
	inputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"source.txt"));
	inputDecoder.Assign(inputFile.GetInputStream());

	bool isSame = true;
	int i=0;
	string::iterator it = aString.begin();

	for(; it != aString.end() && isSame;++it,++i)
	{
		LongBufferSizeType amountRead = inputDecoder.Read(&buffer,1);

		if(amountRead != 1)
		{
			cout<<"Read failes. expected "<<1<<" characters. Found"<<amountRead<<"\n";
			isSame = false;
			break;
		}
		isSame = (*it) == buffer;

	}

	inputDecoder.Assign(NULL);
	inputFile.CloseFile();

	if(!isSame)
	{
		cout<<"Read failes. different characters than what expected\n";
		return PDFHummus::eFailure;
	}
	else
		return PDFHummus::eSuccess;

}

ADD_CATEGORIZED_TEST(InputFlateDecodeTester,"PDFEmbedding")


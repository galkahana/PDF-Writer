#include "InputFlateDecodeTester.h"
#include "InputFlateDecodeStream.h"
#include "OutputFlateEncodeStream.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;

InputFlateDecodeTester::InputFlateDecodeTester(void)
{
}

InputFlateDecodeTester::~InputFlateDecodeTester(void)
{
}

EStatusCode InputFlateDecodeTester::Run()
{
	OutputFile outputFile;
	string aString("hello world");
	IOBasicTypes::Byte buffer;
	InputFlateDecodeStream inputDecoder;
	OutputFlateEncodeStream outputEncoder;

	outputFile.OpenFile(L"C:\\pdflibtests\\source.txt");
	outputEncoder.Assign(outputFile.GetOutputStream());
	outputEncoder.Write((IOBasicTypes::Byte*)aString.c_str(),aString.size());
	outputEncoder.Assign(NULL);
	outputFile.CloseFile();

	InputFile inputFile;
	inputFile.OpenFile(L"C:\\pdflibtests\\source.txt");
	inputDecoder.Assign(inputFile.GetInputStream());

	bool isSame = true;
	int i=0;
	string::iterator it = aString.begin();

	for(; it != aString.end() && isSame;++it,++i)
	{
		LongBufferSizeType amountRead = inputDecoder.Read(&buffer,1);

		if(amountRead != 1)
		{
			wcout<<"Read failes. expected "<<1<<" characters. Found"<<amountRead<<"\n";
			isSame = false;
			break;
		}
		isSame = (*it) == buffer;

	}

	inputDecoder.Assign(NULL);
	inputFile.CloseFile();

	if(!isSame)
	{
		wcout<<"Read failes. different characters than what expected\n";
		return eFailure;
	}
	else
		return eSuccess;

}

ADD_CATEGORIZED_TEST(InputFlateDecodeTester,"PDFEmbedding")


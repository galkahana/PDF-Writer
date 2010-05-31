#include "OutputFileStreamTest.h"
#include "OutputFileStream.h"

#include <iostream>

using namespace std;
using namespace IOBasicTypes;

static const std::wstring scOutputFileStreamTest = L"OutputFileStreamTest";


OutputFileStreamTest::OutputFileStreamTest(void)
{
}

OutputFileStreamTest::~OutputFileStreamTest(void)
{
}

EStatusCode OutputFileStreamTest::Run()
{
	OutputFileStream stream(L"C:\\PDFLibTests\\OutputFileStreamTest.txt");
	Byte buffer[5] = {'a','b','c','d','e'};

	stream.Write(buffer,5);
	stream.Close();

	return eSuccess;
}

ADD_CETEGORIZED_TEST(OutputFileStreamTest,"IO")


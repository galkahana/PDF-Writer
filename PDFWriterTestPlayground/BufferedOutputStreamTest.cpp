#include "BufferedOutputStreamTest.h"
#include "BufferedOutputStream.h"
#include "OutputFileStream.h"

#include <iostream>

using namespace std;

static const std::wstring scBufferedOutputStreamTest = L"BufferedOutputStreamTest";

BufferedOutputStreamTest::BufferedOutputStreamTest(void)
{
}

BufferedOutputStreamTest::~BufferedOutputStreamTest(void)
{
}

EStatusCode BufferedOutputStreamTest::Run()
{
	IByteWriter* stream = new BufferedOutputStream(new OutputFileStream(L"C:\\PDFLibTests\\BufferedOutputStreamTest.txt"),2);
	Byte buffer[5] = {'a','b','c','d','e'};

	stream->Write(buffer,5);
	delete stream;

	return eSuccess;
}

ADD_CETEGORIZED_TEST(BufferedOutputStreamTest,"IO")
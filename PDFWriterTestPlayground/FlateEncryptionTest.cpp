#include "FlateEncryptionTest.h"

#include "InputBufferedStream.h"
#include "OutputBufferedStream.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "OutputFlateDecodeStream.h"
#include "OutputFlateEncodeStream.h"
#include "OutputStreamTraits.h"

#include <iostream>

using namespace std;
using namespace IOBasicTypes;

FlateEncryptionTest::FlateEncryptionTest(void)
{
}

FlateEncryptionTest::~FlateEncryptionTest(void)
{
}

EStatusCode FlateEncryptionTest::Run()
{
	EStatusCode status;

	do
	{
		// Create encrypted copy of the message
		char* aString = "encryptedMessage";

		IByteWriter* encoderStream = new OutputFlateEncodeStream(new OutputBufferedStream(new OutputFileStream(L"c:\\PDFLibTests\\encrypted.txt")));

		LongBufferSizeType writtenSize = encoderStream->Write((const IOBasicTypes::Byte*)aString,strlen(aString));
		delete encoderStream;
		if(writtenSize != strlen(aString))
		{
			wcout<<"Failed to write all message to output\n";
			status = eFailure;
			break;
		}

		IByteReader* encoderReaderStream = new InputBufferedStream(new InputFileStream(L"c:\\PDFLibTests\\encrypted.txt"));
		IByteWriter* decoderWriterStream = new OutputFlateDecodeStream(new OutputBufferedStream(new OutputFileStream(L"c:\\PDFLibTests\\decrypted.txt"))); 

		OutputStreamTraits outputTraits(decoderWriterStream);
		status = outputTraits.CopyToOutputStream(encoderReaderStream);
		if(status != eSuccess)
		{
			wcout<<"Failed to copy to decrypted output\n";
			status = eFailure;
			break;
		}

		delete encoderReaderStream;
		delete decoderWriterStream;

		// now read again decrypted and compare to original message
		IByteReader* decoderReaderStream = new InputBufferedStream(new InputFileStream(L"c:\\PDFLibTests\\decrypted.txt"));
		char buffer[256];

		LongBufferSizeType readSize = decoderReaderStream->Read((IOBasicTypes::Byte*)buffer,255);
		buffer[readSize] = 0;

		if(strcmp(aString,buffer) != 0)
		{
			wcout<<"decrypted content is different from encrypted content\n";
			status = eFailure;
			break;
		}
	}while(false);

	return status;
}

ADD_CETEGORIZED_TEST(FlateEncryptionTest,"IO")

/*
   Source File : FlateEncryptionTest.cpp


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
#include "InputBufferedStream.h"
#include "OutputBufferedStream.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "OutputFlateDecodeStream.h"
#include "OutputFlateEncodeStream.h"
#include "OutputStreamTraits.h"

#include <iostream>
#include <string.h>

#include "testing/TestIO.h"

using namespace std;
using namespace IOBasicTypes;
using namespace PDFHummus;

int FlateEncryptionTest(int argc, char* argv[])
{
	EStatusCode status;

	do
	{
		// Create encrypted copy of the message
		string aString = "encryptedMessage";

		IByteWriter* encoderStream = new OutputFlateEncodeStream(new OutputBufferedStream(new OutputFileStream(
                                        BuildRelativeOutputPath(argv,"encrypted.txt"))));

		LongBufferSizeType writtenSize = encoderStream->Write((const IOBasicTypes::Byte*)aString.c_str(),aString.length());
		delete encoderStream;
		if(writtenSize != aString.length())
		{
			cout<<"Failed to write all message to output\n";
			status = PDFHummus::eFailure;
			break;
		}

		IByteReader* encoderReaderStream = new InputBufferedStream(new InputFileStream(
                                          BuildRelativeOutputPath(argv,"encrypted.txt")));
		IByteWriter* decoderWriterStream = new OutputFlateDecodeStream(new OutputBufferedStream(new OutputFileStream(
                                          BuildRelativeOutputPath(argv,"decrypted.txt")))); 

		OutputStreamTraits outputTraits(decoderWriterStream);
		status = outputTraits.CopyToOutputStream(encoderReaderStream);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to copy to decrypted output\n";
			status = PDFHummus::eFailure;
			break;
		}

		delete encoderReaderStream;
		delete decoderWriterStream;

		// now read again decrypted and compare to original message
		IByteReader* decoderReaderStream = new InputBufferedStream(new InputFileStream(
                                          BuildRelativeOutputPath(argv,"decrypted.txt")));
		char buffer[256];

		LongBufferSizeType readSize = decoderReaderStream->Read((IOBasicTypes::Byte*)buffer,255);
		buffer[readSize] = 0;

		delete decoderReaderStream;

		if(strcmp(aString.c_str(),buffer) != 0)
		{
			cout<<"decrypted content is different from encrypted content\n";
			status = PDFHummus::eFailure;
			break;
		}
	}while(false);

	return status == eSuccess ? 0:1;
}

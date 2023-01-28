/*
   Source File : BufferedOutputStreamTest.cpp


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
#include "OutputBufferedStream.h"
#include "OutputFileStream.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace std;
using namespace IOBasicTypes;
using namespace PDFHummus;

static const std::string scBufferedOutputStreamTest = "BufferedOutputStreamTest";


int BufferedOutputStreamTest(int argc, char* argv[])
{
	IByteWriter* stream = new OutputBufferedStream(new OutputFileStream(
                            BuildRelativeOutputPath(argv,"BufferedOutputStreamTest.txt")),2);
	Byte buffer[5] = {'a','b','c','d','e'};

	stream->Write(buffer,5);
	delete stream;

	return 0;
}

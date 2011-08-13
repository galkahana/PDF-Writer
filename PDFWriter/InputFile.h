/*
   Source File : InputFile.h


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

#include "EPDFStatusCode.h"
#include "IByteReaderWithPosition.h"
#include <string>

class InputBufferedStream;
class InputFileStream;

using namespace std;

class InputFile
{
public:
	InputFile(void);
	~InputFile(void);

	EPDFStatusCode OpenFile(const string& inFilePath);
	EPDFStatusCode CloseFile();

	IByteReaderWithPosition* GetInputStream(); // returns buffered input stream
	const string& GetFilePath();
	
	LongFilePositionType GetFileSize();

private:
	string mFilePath;
	InputBufferedStream* mInputStream;
	InputFileStream* mFileStream;
};

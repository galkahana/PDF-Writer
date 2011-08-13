/*
   Source File : OutputFile.h


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
#include <string>

class IByteWriterWithPosition;
class OutputBufferedStream;
class OutputFileStream;

using namespace std;

class OutputFile
{
public:
	OutputFile(void);
	~OutputFile(void);

	EPDFStatusCode OpenFile(const string& inFilePath, bool inAppend = false);
	EPDFStatusCode CloseFile();

	IByteWriterWithPosition* GetOutputStream(); // returns buffered output stream
	const string& GetFilePath();
private:
	string mFilePath;
	OutputBufferedStream* mOutputStream;
	OutputFileStream* mFileStream;
};

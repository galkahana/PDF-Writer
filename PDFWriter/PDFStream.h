/*
   Source File : PDFStream.h


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
/*
	PDFStream objects represents a stream in the PDF.
	due to @#$@$ Length key in stream dictionary, stream writing in the library is a two step matter.
	first use PDFStream to write the stream content, then write the Stream to the PDF using ObjectContext WriteStream
*/

#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include "ObjectsBasicTypes.h"
#include "OutputFlateEncodeStream.h"
#include <sstream>


using namespace IOBasicTypes;
using namespace std;

class IByteWriterWithPosition;
class IObjectsContextExtender;

class PDFStream
{
public:
	PDFStream(
		bool inCompressStream,
		IByteWriterWithPosition* inOutputStream,
		ObjectIDType inExtentObjectID,
		IObjectsContextExtender* inObjectsContextExtender);
	~PDFStream(void);

	// Get the output stream of the PDFStream, make sure to use only before calling FinalizeStreamWrite, after which it becomes invalid
	IByteWriter* GetWriteStream();

	// when done with writing to the stream call FinalizeWriteStream to get all writing resources released and calculate the stream extent
	void FinalizeStreamWrite();

	bool IsStreamCompressed();
	ObjectIDType GetExtentObjectID();

	LongFilePositionType GetLength(); // get the stream extent

private:
	bool mCompressStream;
	OutputFlateEncodeStream mFlateEncodingStream;
	IByteWriterWithPosition* mOutputStream;
	ObjectIDType mExtendObjectID;
	LongFilePositionType mStreamLength;
	LongFilePositionType mStreamStartPosition;
	IByteWriter* mWriteStream;
	IObjectsContextExtender* mExtender;
};

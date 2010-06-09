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

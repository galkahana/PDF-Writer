#pragma once
/*
	PDFStream objects represents a stream in the PDF.
	due to @#$@$ Length key in stream dictionary, stream writing in the library is a two step matter.
	first use PDFStream to write the stream content, then write the Stream to the PDF using ObjectContext WriteStream
*/

#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include <sstream>


using namespace IOBasicTypes;
using namespace std;

class InputStringBufferStream;
class OutputStringBufferStream;
class IByteWriter;
class IByteReader;

class PDFStream
{
public:
	PDFStream(void);
	~PDFStream(void);

	// Get the output stream of the PDFStream, make sure to use only before calling FinalizeWriteAndStratRead, after which it becomes invalid
	IByteWriter* GetWriteStream();

	// Internal, used by ObjectsContext to implement the writing of stream to the main PDF stream
	EStatusCode FinalizeWriteAndStratRead();
	LongFilePositionType GetLength();
	IByteReader* GetReadStream();

private:
	stringbuf mIOString;
	LongFilePositionType mStreamLength;
	InputStringBufferStream* mReadStream;
	OutputStringBufferStream* mWriteStream;

};

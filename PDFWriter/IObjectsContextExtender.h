#pragma once

class IByteWriter;
class IByteWriterWithPosition;

class IObjectsContextExtender
{

public:
	virtual ~IObjectsContextExtender(){}

	// PDFStream object compression extensibility

	// flag to determine if the extender takes over compression or not
	// if true than other compression methods will be called in case compression is required
	virtual bool OverridesStreamCompression() = 0;
	
	// GetCompressionWriteStream is called when PDFStream Object is created 
	// if compression was required and OverridesStreamCompression returned true.
	// input is the stream to write to, output is the new stream to write to - the compressed stream
	virtual IByteWriter* GetCompressionWriteStream(IByteWriterWithPosition* inOutputStream) = 0;

	// FinalizeCompressedStreamWrite is called when the PDFStream is finalized.
	// it recieves as input the compressed stream that was returned from GetCompressionWriteStream.
	// the implementation should finalize the compression stream write [any remaning footer buffers should be written now].
	// this would allow the PDFStream to calculate the extent on the actual write stream (given as input for GetCompressionWriteStream).
	virtual void FinalizeCompressedStreamWrite(IByteWriter* inCompressedStream) = 0;

};
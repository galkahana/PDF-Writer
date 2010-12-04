#pragma once

/*
	reads a PFB stream, decoding the eexec parts. A type 1 parser can use this stream to make the parsing
	more trivial, not requiring to keep track on which area one's in.
*/

#include "EStatusCode.h"
#include "IByteReader.h"

#include <utility>
#include <string>

using namespace std;

class InputPFBDecodeStream;

typedef EStatusCode (*DecodeMethod)(
	InputPFBDecodeStream* inThis,
	Byte& outByte 
 );

typedef pair<bool,string> BoolAndString;

class InputPFBDecodeStream : IByteReader
{
public:
	InputPFBDecodeStream(void);
	~InputPFBDecodeStream(void);

	// Assign will set the stream to decode. it also takes ownership of the stream. if you
	// don't want the ownership make sure to Assign(NULL) when done using the decoder.
	EStatusCode Assign(IByteReader* inStreamToDecode);
	
	// IByteReader implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

	// token actions

	// get the next avialable postscript token. return result returns whether
	// token retreive was successful and the token (if it was).
	// note that segment end automatically cuts of a token
	BoolAndString GetNextToken();

	// skip white spaces till token or EOF. note that end of segment 
	// will stop tokenizer as well
	void SkipTillToken();


	// internal usage.
	EStatusCode ReadDecodedByte(Byte& outByte);
	EStatusCode ReadRegularByte(Byte& outByte);

private:
	IByteReader* mStreamToDecode;
	LongFilePositionType mInSegmentReadIndex;
	LongFilePositionType mSegmentSize;
	Byte mCurrentType;
	DecodeMethod mDecodeMethod;
	bool mHasTokenBuffer;
	Byte mTokenBuffer;
	unsigned short mRandomizer;
	bool mFoundEOF;
	
	// error flag. if set, will not allow further reading
	EStatusCode mInternalState; 

	// Starts reading a stream segment, to receive the type and length of the segment
	EStatusCode InitializeStreamSegment();
	void ResetReadStatus();
	EStatusCode StoreSegmentLength();
	EStatusCode FlushBinarySectionTrailingCode();
	bool IsPostScriptWhiteSpace(Byte inCharacter);
	bool IsSegmentNotEnded();
	void SaveTokenBuffer(Byte inToSave);
	bool IsPostScriptEntityBreaker(Byte inCharacter);
	EStatusCode InitializeBinaryDecode();
	Byte DecodeByte(Byte inByteToDecode);

	EStatusCode GetNextByteForToken(Byte& outByte);

};

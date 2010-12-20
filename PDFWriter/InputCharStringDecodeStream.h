#pragma once
#include "IByteReader.h"
#include "EStatusCode.h"
 
class InputCharStringDecodeStream : public IByteReader
{
public:
	InputCharStringDecodeStream(IByteReader* inReadFrom,unsigned long inLenIV=4);
	~InputCharStringDecodeStream(void);

	void Assign(IByteReader* inReadFrom,unsigned long inLenIV=4);

	// IByteReader implementation

	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

private:
	IByteReader* mReadFrom;
	unsigned short mRandomizer;


	void InitializeCharStringDecode(unsigned long inLenIV);
	EStatusCode ReadDecodedByte(Byte& outByte);
	Byte DecodeByte(Byte inByteToDecode);

};

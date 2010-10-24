#pragma once

#include "EStatusCode.h"
#include "IOBasicTypes.h"

class IByteWriter;
class IByteReader;

using namespace IOBasicTypes;

class OutputStreamTraits
{
public:
	OutputStreamTraits(IByteWriter* inOutputStream);
	~OutputStreamTraits(void);


	EStatusCode CopyToOutputStream(IByteReader* inInputStream);	
	EStatusCode CopyToOutputStream(IByteReader* inInputStream,LongBufferSizeType inLength);	

private:

	IByteWriter* mOutputStream;
};

#pragma once

#include "EStatusCode.h"

class IByteWriter;
class IByteReader;

class OutputStreamTraits
{
public:
	OutputStreamTraits(IByteWriter* inOutputStream);
	~OutputStreamTraits(void);


	EStatusCode CopyToOutputStream(IByteReader* inInputStream);	

private:

	IByteWriter* mOutputStream;
};

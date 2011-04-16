#pragma once

#include "IReadPositionProvider.h"
#include "IByteReaderWithPosition.h"

class AdapterIByteReaderWithPositionToIReadPositionProvider : public IReadPositionProvider
{
public:
	AdapterIByteReaderWithPositionToIReadPositionProvider(){mStream = NULL;}
	AdapterIByteReaderWithPositionToIReadPositionProvider(IByteReaderWithPosition* inStream){mStream = inStream;}
	
	void Assign(IByteReaderWithPosition* inStream){mStream = inStream;}

	// IReadPositionProvider implementation
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition(){return mStream->GetCurrentPosition();};

private:
	IByteReaderWithPosition* mStream;
	
};
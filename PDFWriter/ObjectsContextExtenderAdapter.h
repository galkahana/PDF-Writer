#pragma once

#include "IObjectsContextExtender.h"

class ObjectsContextExtenderAdapter
{
public:

	virtual bool OverridesStreamCompression(){return false;}
	
	virtual IByteWriter* GetCompressionWriteStream(IByteWriterWithPosition* inOutputStream){return inOutputStream;}

	virtual void FinalizeCompressedStreamWrite(IByteWriter* inCompressedStream){}

protected:
	ObjectsContextExtenderAdapter(){}

};
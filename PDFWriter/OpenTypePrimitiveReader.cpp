#include "OpenTypePrimitiveReader.h"

OpenTypePrimitiveReader::OpenTypePrimitiveReader(IByteReader* inOpenTypeFile)
{
	SetOpenTypeStream(inOpenTypeFile);
}

OpenTypePrimitiveReader::~OpenTypePrimitiveReader(void)
{
}

void OpenTypePrimitiveReader::SetOpenTypeStream(IByteReader* inOpenTypeFile)
{
	mOpenTypeFile = inOpenTypeFile;
	if(inOpenTypeFile)
	{
		mInitialPosition = inOpenTypeFile->GetCurrentPosition();
		mInternalState = eSuccess;
	}
	else
	{
		mInternalState = eFailure;
	}
}

EStatusCode OpenTypePrimitiveReader::ReadBYTE(unsigned char& outValue)
{
	if(eFailure == mInternalState)
		return eFailure;

	Byte buffer;
	EStatusCode status = (mOpenTypeFile->Read(&buffer,1) == 1 ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	outValue = buffer;
	return status;
}

EStatusCode OpenTypePrimitiveReader::ReadCHAR(char& outValue)
{
	Byte buffer;

	if(ReadBYTE(buffer) != eSuccess)
		return eFailure;

	outValue = (char)buffer;
	return eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadUSHORT(unsigned short& outValue)
{
	Byte byte1,byte2;

	if(ReadBYTE(byte1) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte2) != eSuccess)
		return eFailure;


	outValue = ((unsigned short)byte1 << 8) + byte2;

	return eSuccess;

}

EStatusCode OpenTypePrimitiveReader::ReadSHORT(short& outValue)
{
	unsigned short buffer;

	if(ReadUSHORT(buffer) != eSuccess)
		return eFailure;

	outValue = (short)buffer;
	return eSuccess;

}

EStatusCode OpenTypePrimitiveReader::ReadULONG(unsigned long& outValue)
{
	Byte byte1,byte2,byte3,byte4;

	if(ReadBYTE(byte1) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte2) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte3) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte4) != eSuccess)
		return eFailure;

	outValue = ((unsigned long)byte1 << 24) + ((unsigned long)byte2 << 16) + 
									((unsigned long)byte3 << 8) + byte4;

	return eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadLONG(long& outValue)
{
	unsigned long buffer;

	if(ReadULONG(buffer) != eSuccess)
		return eFailure;

	outValue = (long)buffer;
	return eSuccess;
}

EStatusCode OpenTypePrimitiveReader::ReadLongDateTime(long long& outValue)
{
	Byte byte1,byte2,byte3,byte4,byte5,byte6,byte7,byte8;

	if(ReadBYTE(byte1) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte2) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte3) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte4) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte5) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte6) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte7) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte8) != eSuccess)
		return eFailure;

	outValue =(long long)(	((unsigned long long)byte1 << 56) + ((unsigned long long)byte2 << 48) + 
							((unsigned long long)byte3 << 40) + ((unsigned long long)byte4 << 32) + 
							((unsigned long long)byte5 << 24) + ((unsigned long long)byte6 << 16) + 
							((unsigned long long)byte7 << 8) + byte8);
	return eSuccess;	
}

void OpenTypePrimitiveReader::SetOffset(LongFilePositionType inNewOffset)
{
	if(mInternalState != eFailure)
		mOpenTypeFile->SetPosition(mInitialPosition + inNewOffset);
}	

void OpenTypePrimitiveReader::Skip(LongBufferSizeType inToSkip)
{
	if(mInternalState != eFailure)
		mOpenTypeFile->Skip(inToSkip);
}

EStatusCode OpenTypePrimitiveReader::GetInternalState()
{
	return mInternalState;
}

EStatusCode OpenTypePrimitiveReader::ReadFixed(double& outValue)
{
	unsigned short integer,fraction;

	if(ReadUSHORT(integer) != eSuccess)
		return eFailure;

	if(ReadUSHORT(fraction) != eSuccess)
		return eFailure;

	outValue = (double)integer + ((double)fraction) / (1<<16);

	return eSuccess;
}

LongFilePositionType OpenTypePrimitiveReader::GetCurrentPosition()
{
	if(mInternalState != eFailure)
		return mOpenTypeFile->GetCurrentPosition() - mInitialPosition;	
	else
		return 0;
}

EStatusCode OpenTypePrimitiveReader::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(eFailure == mInternalState)
		return eFailure;

	EStatusCode status = (mOpenTypeFile->Read(inBuffer,inBufferSize) == inBufferSize ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	return status;	
}

IByteReader* OpenTypePrimitiveReader::GetReadStream()
{
	return mOpenTypeFile;
}
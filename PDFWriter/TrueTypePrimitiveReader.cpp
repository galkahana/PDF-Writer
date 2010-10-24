#include "TrueTypePrimitiveReader.h"

TrueTypePrimitiveReader::TrueTypePrimitiveReader(IByteReader* inTrueTypeFile)
{
	SetTrueTypeStream(inTrueTypeFile);
}

TrueTypePrimitiveReader::~TrueTypePrimitiveReader(void)
{
}

void TrueTypePrimitiveReader::SetTrueTypeStream(IByteReader* inTrueTypeFile)
{
	mTrueTypeFile = inTrueTypeFile;
	if(inTrueTypeFile)
	{
		mInitialPosition = inTrueTypeFile->GetCurrentPosition();
		mInternalState = eSuccess;
	}
	else
	{
		mInternalState = eFailure;
	}
}

EStatusCode TrueTypePrimitiveReader::ReadBYTE(unsigned char& outValue)
{
	if(eFailure == mInternalState)
		return eFailure;

	Byte buffer;
	EStatusCode status = (mTrueTypeFile->Read(&buffer,1) == 1 ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	outValue = buffer;
	return status;
}

EStatusCode TrueTypePrimitiveReader::ReadCHAR(char& outValue)
{
	Byte buffer;

	if(ReadBYTE(buffer) != eSuccess)
		return eFailure;

	outValue = (char)buffer;
	return eSuccess;
}

EStatusCode TrueTypePrimitiveReader::ReadUSHORT(unsigned short& outValue)
{
	Byte byte1,byte2;

	if(ReadBYTE(byte1) != eSuccess)
		return eFailure;

	if(ReadBYTE(byte2) != eSuccess)
		return eFailure;


	outValue = ((unsigned short)byte1 << 8) + byte2;

	return eSuccess;

}

EStatusCode TrueTypePrimitiveReader::ReadSHORT(short& outValue)
{
	unsigned short buffer;

	if(ReadUSHORT(buffer) != eSuccess)
		return eFailure;

	outValue = (short)buffer;
	return eSuccess;

}

EStatusCode TrueTypePrimitiveReader::ReadULONG(unsigned long& outValue)
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

EStatusCode TrueTypePrimitiveReader::ReadLONG(long& outValue)
{
	unsigned long buffer;

	if(ReadULONG(buffer) != eSuccess)
		return eFailure;

	outValue = (long)buffer;
	return eSuccess;
}

EStatusCode TrueTypePrimitiveReader::ReadLongDateTime(long long& outValue)
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

void TrueTypePrimitiveReader::SetOffset(LongFilePositionType inNewOffset)
{
	if(mInternalState != eFailure)
		mTrueTypeFile->SetPosition(mInitialPosition + inNewOffset);
}	

void TrueTypePrimitiveReader::Skip(LongBufferSizeType inToSkip)
{
	if(mInternalState != eFailure)
		mTrueTypeFile->Skip(inToSkip);
}

EStatusCode TrueTypePrimitiveReader::GetInternalState()
{
	return mInternalState;
}

EStatusCode TrueTypePrimitiveReader::ReadFixed(double& outValue)
{
	unsigned short integer,fraction;

	if(ReadUSHORT(integer) != eSuccess)
		return eFailure;

	if(ReadUSHORT(fraction) != eSuccess)
		return eFailure;

	outValue = (double)integer + ((double)fraction) / (1<<16);

	return eSuccess;
}

LongFilePositionType TrueTypePrimitiveReader::GetCurrentPosition()
{
	if(mInternalState != eFailure)
		return mTrueTypeFile->GetCurrentPosition() - mInitialPosition;	
	else
		return 0;
}

EStatusCode TrueTypePrimitiveReader::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(eFailure == mInternalState)
		return eFailure;

	EStatusCode status = (mTrueTypeFile->Read(inBuffer,inBufferSize) == inBufferSize ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	return status;	
}
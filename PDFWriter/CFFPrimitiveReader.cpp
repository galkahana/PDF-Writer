#include "CFFPrimitiveReader.h"
#include <math.h>

CFFPrimitiveReader::CFFPrimitiveReader(IByteReader* inCFFFile)
{
	SetStream(inCFFFile);
}

CFFPrimitiveReader::~CFFPrimitiveReader(void)
{
}

void CFFPrimitiveReader::SetStream(IByteReader* inCFFFile)
{
	mCFFFile = inCFFFile;
	if(inCFFFile)
	{
		mCurrentOffsize = 1;
		mInitialPosition = inCFFFile->GetCurrentPosition();
		mInternalState = eSuccess;
	}
	else
	{
		mInternalState = eFailure;
	}
}


void CFFPrimitiveReader::SetOffset(LongFilePositionType inNewOffset)
{
	if(mInternalState != eFailure)
		mCFFFile->SetPosition(mInitialPosition + inNewOffset);
}	

void CFFPrimitiveReader::Skip(LongBufferSizeType inToSkip)
{
	if(mInternalState != eFailure)
		mCFFFile->Skip(inToSkip);
}

EStatusCode CFFPrimitiveReader::GetInternalState()
{
	return mInternalState;
}

LongFilePositionType CFFPrimitiveReader::GetCurrentPosition()
{
	if(mInternalState != eFailure)
		return mCFFFile->GetCurrentPosition() - mInitialPosition;	
	else
		return 0;
}

EStatusCode CFFPrimitiveReader::ReadByte(Byte& outValue)
{
	if(eFailure == mInternalState)
		return eFailure;

	Byte buffer;
	EStatusCode status = (mCFFFile->Read(&buffer,1) == 1 ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	outValue = buffer;
	return status;	
}

EStatusCode CFFPrimitiveReader::Read(Byte* ioBuffer,LongBufferSizeType inBufferSize)
{
	if(eFailure == mInternalState)
		return eFailure;

	EStatusCode status = (mCFFFile->Read(ioBuffer,inBufferSize) == inBufferSize ? eSuccess : eFailure);

	if(eFailure == status)
		mInternalState = eFailure;
	return status;	
}

EStatusCode CFFPrimitiveReader::ReadCard8(Byte& outValue)
{
	return ReadByte(outValue);
}

EStatusCode CFFPrimitiveReader::ReadCard16(unsigned short& outValue)
{
	Byte byte1,byte2;

	if(ReadByte(byte1) != eSuccess)
		return eFailure;

	if(ReadByte(byte2) != eSuccess)
		return eFailure;


	outValue = ((unsigned short)byte1 << 8) + byte2;

	return eSuccess;

}
EStatusCode CFFPrimitiveReader::Read2ByteSigned(short& outValue)
{
	unsigned short buffer;
	EStatusCode status = ReadCard16(buffer);

	if(status != eSuccess)
		return eFailure;

	outValue = (short)buffer;
	return eSuccess;
}


void CFFPrimitiveReader::SetOffSize(Byte inOffSize)
{
	mCurrentOffsize = inOffSize;
}


EStatusCode CFFPrimitiveReader::ReadOffset(unsigned long& outValue)
{
	EStatusCode status = eFailure;

	switch(mCurrentOffsize)
	{
		case 1:
			Byte byteBuffer;
			status = ReadCard8(byteBuffer);
			if(eSuccess == status)
				outValue = byteBuffer;
			break;
		case 2:
			unsigned short shortBuffer;
			status = ReadCard16(shortBuffer);
			if(eSuccess == status)
				outValue = shortBuffer;
			break;
		case 3:
			status = Read3ByteUnsigned(outValue);
			break;
		case 4:
			status = Read4ByteUnsigned(outValue);
			break;

	}

	return status;
}

EStatusCode CFFPrimitiveReader::Read3ByteUnsigned(unsigned long& outValue)
{
	Byte byte1,byte2,byte3;

	if(ReadByte(byte1) != eSuccess)
		return eFailure;

	if(ReadByte(byte2) != eSuccess)
		return eFailure;

	if(ReadByte(byte3) != eSuccess)
		return eFailure;

	outValue = ((unsigned long)byte1 << 16) + ((unsigned long)byte2 << 8) + byte3;

	return eSuccess;	
}

EStatusCode CFFPrimitiveReader::Read4ByteUnsigned(unsigned long& outValue)
{
	Byte byte1,byte2,byte3,byte4;

	if(ReadByte(byte1) != eSuccess)
		return eFailure;

	if(ReadByte(byte2) != eSuccess)
		return eFailure;

	if(ReadByte(byte3) != eSuccess)
		return eFailure;

	if(ReadByte(byte4) != eSuccess)
		return eFailure;

	outValue = ((unsigned long)byte1 << 24) + 
				((unsigned long)byte2 << 16) + 
					((unsigned long)byte3 << 8) + 
											byte4;

	return eSuccess;	
}

EStatusCode CFFPrimitiveReader::Read4ByteSigned(long& outValue)
{
	unsigned long buffer;
	EStatusCode status = Read4ByteUnsigned(buffer);

	if(status != eSuccess)
		return eFailure;

	outValue = (long)buffer;

	return eSuccess;
}


EStatusCode CFFPrimitiveReader::ReadOffSize(Byte& outValue)
{
	return ReadCard8(outValue);
}

EStatusCode CFFPrimitiveReader::ReadSID(unsigned short& outValue)
{
	return ReadCard16(outValue);
}

EStatusCode CFFPrimitiveReader::ReadIntegerOperand(Byte inFirstByte,long& outValue)
{
	Byte byte0,byte1;
	EStatusCode status = eSuccess;

	byte0 = inFirstByte;

	if(byte0 >= 32 && byte0 <= 246)
	{
		outValue = (long)byte0 - 139;		
	}
	else if(byte0 >= 247 && byte0 <= 250)
	{
		if(ReadByte(byte1) != eSuccess)
			return eFailure;

		outValue = (byte0-247) * 256 + byte1 + 108;
	} 
	else if (byte0 >= 251 && byte0 <= 254)
	{
		if(ReadByte(byte1) != eSuccess)
			return eFailure;

		outValue = -(long)((long)byte0-251) * 256 - byte1 - 108;
	}
	else if (28 == byte0)
	{
		short buffer = 0;
		status = Read2ByteSigned(buffer);
		outValue = buffer;
	} 
	else if(29 == byte0)
	{
		status = Read4ByteSigned(outValue);
	}
	else
		status = eFailure;

	return status;
}

EStatusCode CFFPrimitiveReader::ReadRealOperand(double& outValue,long& outRealValueFractalEnd)
{
	double integerPart = 0;
	double fractionPart = 0;
	double powerPart = 0;
	double result;
	bool hasNegative = false;
	bool hasFraction = false;
	bool hasPositivePower = false;
	bool hasNegativePower = false;
	bool notDone = true;
	double fractionDecimal = 1;
	outRealValueFractalEnd = 0;
	Byte buffer;
	Byte nibble[2];
	EStatusCode status = eSuccess;

	do
	{
		status = ReadByte(buffer);
		if(status != eSuccess)
			break;

		nibble[0] = (buffer >> 4) & 0xf;
		nibble[1] = buffer & 0xf;

		for(int i = 0; i <2; ++i)
		{
			switch(nibble[i])
			{
				case 0xa:
					hasFraction = true;
					break;
				case 0xb:
					hasPositivePower = true;
					break;
				case 0xc:
					hasNegativePower = true;
					break;
				case 0xd:
					// reserved
					break;
				case 0xe:
					hasNegative = true;
					break;
				case 0xf:
					notDone = false;
					break;
				default: // numbers
					if(hasPositivePower || hasNegativePower)
					{
						powerPart = powerPart*10 + nibble[i];
					}
					else if(hasFraction)
					{
						fractionPart = fractionPart * 10 + nibble[i];
						fractionDecimal *= 10;
						++outRealValueFractalEnd;
					}
					else
						integerPart = integerPart * 10 + nibble[i];

			}
		}
	}while(notDone);

	if(eSuccess == status)
	{
		result = integerPart + fractionPart/fractionDecimal;
		if(hasNegativePower || hasPositivePower)
			result = result * pow(10,hasNegativePower ? -powerPart : powerPart);
		if(hasNegative)
			result = -1*result;
		outValue = result;
	}
	return status;
}

bool CFFPrimitiveReader::IsDictOperator(Byte inCandidate)
{
	return ((inCandidate >= 0 && inCandidate <= 27) || 31 == inCandidate);
}	

EStatusCode CFFPrimitiveReader::ReadDictOperator(Byte inFirstByte,unsigned short& outOperator)
{
	if(12 == inFirstByte)
	{
		Byte buffer;
		if(ReadByte(buffer) == eSuccess)
		{	
			outOperator = ((unsigned short)inFirstByte << 8) | buffer;
			return eSuccess;
		}
		else
			return eFailure;
	}
	else
	{
		outOperator = inFirstByte;
		return eSuccess;
	}
}

EStatusCode CFFPrimitiveReader::ReadDictOperand(Byte inFirstByte,DictOperand& outOperand)
{
	if(30 == inFirstByte) // real
	{
		outOperand.IsInteger = false;
		return ReadRealOperand(outOperand.RealValue,outOperand.RealValueFractalEnd);
	}
	else if(28 == inFirstByte ||
			29 == inFirstByte ||
			(32 <= inFirstByte && inFirstByte <= 246) ||
			(247 <= inFirstByte && inFirstByte <= 250) ||
			(251 <= inFirstByte && inFirstByte <= 254))
	{
		outOperand.IsInteger = true;
		return ReadIntegerOperand(inFirstByte,outOperand.IntegerValue);
	}
	else
		return eFailure; // not an operand
}

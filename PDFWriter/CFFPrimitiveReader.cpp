/*
   Source File : CFFPrimitiveReader.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "CFFPrimitiveReader.h"
#include <math.h>

CFFPrimitiveReader::CFFPrimitiveReader(IByteReaderWithPosition* inCFFFile)
{
	SetStream(inCFFFile);
}

CFFPrimitiveReader::~CFFPrimitiveReader(void)
{
}

void CFFPrimitiveReader::SetStream(IByteReaderWithPosition* inCFFFile)
{
	mCFFFile = inCFFFile;
	if(inCFFFile)
	{
		mCurrentOffsize = 1;
		mInitialPosition = inCFFFile->GetCurrentPosition();
		mInternalState = ePDFSuccess;
	}
	else
	{
		mInternalState = ePDFFailure;
	}
}


void CFFPrimitiveReader::SetOffset(LongFilePositionType inNewOffset)
{
	if(mInternalState != ePDFFailure)
		mCFFFile->SetPosition(mInitialPosition + inNewOffset);
}	

void CFFPrimitiveReader::Skip(LongBufferSizeType inToSkip)
{
	if(mInternalState != ePDFFailure)
		mCFFFile->Skip(inToSkip);
}

EPDFStatusCode CFFPrimitiveReader::GetInternalState()
{
	return mInternalState;
}

LongFilePositionType CFFPrimitiveReader::GetCurrentPosition()
{
	if(mInternalState != ePDFFailure)
		return mCFFFile->GetCurrentPosition() - mInitialPosition;	
	else
		return 0;
}

EPDFStatusCode CFFPrimitiveReader::ReadByte(Byte& outValue)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	Byte buffer;
	EPDFStatusCode status = (mCFFFile->Read(&buffer,1) == 1 ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	outValue = buffer;
	return status;	
}

EPDFStatusCode CFFPrimitiveReader::Read(Byte* ioBuffer,LongBufferSizeType inBufferSize)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	EPDFStatusCode status = (mCFFFile->Read(ioBuffer,inBufferSize) == inBufferSize ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	return status;	
}

EPDFStatusCode CFFPrimitiveReader::ReadCard8(Byte& outValue)
{
	return ReadByte(outValue);
}

EPDFStatusCode CFFPrimitiveReader::ReadCard16(unsigned short& outValue)
{
	Byte byte1,byte2;

	if(ReadByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte2) != ePDFSuccess)
		return ePDFFailure;


	outValue = ((unsigned short)byte1 << 8) + byte2;

	return ePDFSuccess;

}
EPDFStatusCode CFFPrimitiveReader::Read2ByteSigned(short& outValue)
{
	unsigned short buffer;
	EPDFStatusCode status = ReadCard16(buffer);

	if(status != ePDFSuccess)
		return ePDFFailure;

	outValue = (short)buffer;
	return ePDFSuccess;
}


void CFFPrimitiveReader::SetOffSize(Byte inOffSize)
{
	mCurrentOffsize = inOffSize;
}


EPDFStatusCode CFFPrimitiveReader::ReadOffset(unsigned long& outValue)
{
	EPDFStatusCode status = ePDFFailure;

	switch(mCurrentOffsize)
	{
		case 1:
			Byte byteBuffer;
			status = ReadCard8(byteBuffer);
			if(ePDFSuccess == status)
				outValue = byteBuffer;
			break;
		case 2:
			unsigned short shortBuffer;
			status = ReadCard16(shortBuffer);
			if(ePDFSuccess == status)
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

EPDFStatusCode CFFPrimitiveReader::Read3ByteUnsigned(unsigned long& outValue)
{
	Byte byte1,byte2,byte3;

	if(ReadByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte3) != ePDFSuccess)
		return ePDFFailure;

	outValue = ((unsigned long)byte1 << 16) + ((unsigned long)byte2 << 8) + byte3;

	return ePDFSuccess;	
}

EPDFStatusCode CFFPrimitiveReader::Read4ByteUnsigned(unsigned long& outValue)
{
	Byte byte1,byte2,byte3,byte4;

	if(ReadByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte3) != ePDFSuccess)
		return ePDFFailure;

	if(ReadByte(byte4) != ePDFSuccess)
		return ePDFFailure;

	outValue = ((unsigned long)byte1 << 24) + 
				((unsigned long)byte2 << 16) + 
					((unsigned long)byte3 << 8) + 
											byte4;

	return ePDFSuccess;	
}

EPDFStatusCode CFFPrimitiveReader::Read4ByteSigned(long& outValue)
{
	unsigned long buffer;
	EPDFStatusCode status = Read4ByteUnsigned(buffer);

	if(status != ePDFSuccess)
		return ePDFFailure;

	outValue = (long)buffer;

	return ePDFSuccess;
}


EPDFStatusCode CFFPrimitiveReader::ReadOffSize(Byte& outValue)
{
	return ReadCard8(outValue);
}

EPDFStatusCode CFFPrimitiveReader::ReadSID(unsigned short& outValue)
{
	return ReadCard16(outValue);
}

EPDFStatusCode CFFPrimitiveReader::ReadIntegerOperand(Byte inFirstByte,long& outValue)
{
	Byte byte0,byte1;
	EPDFStatusCode status = ePDFSuccess;

	byte0 = inFirstByte;

	if(byte0 >= 32 && byte0 <= 246)
	{
		outValue = (long)byte0 - 139;		
	}
	else if(byte0 >= 247 && byte0 <= 250)
	{
		if(ReadByte(byte1) != ePDFSuccess)
			return ePDFFailure;

		outValue = (byte0-247) * 256 + byte1 + 108;
	} 
	else if (byte0 >= 251 && byte0 <= 254)
	{
		if(ReadByte(byte1) != ePDFSuccess)
			return ePDFFailure;

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
		status = ePDFFailure;

	return status;
}

EPDFStatusCode CFFPrimitiveReader::ReadRealOperand(double& outValue,long& outRealValueFractalEnd)
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
	EPDFStatusCode status = ePDFSuccess;

	do
	{
		status = ReadByte(buffer);
		if(status != ePDFSuccess)
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

	if(ePDFSuccess == status)
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

EPDFStatusCode CFFPrimitiveReader::ReadDictOperator(Byte inFirstByte,unsigned short& outOperator)
{
	if(12 == inFirstByte)
	{
		Byte buffer;
		if(ReadByte(buffer) == ePDFSuccess)
		{	
			outOperator = ((unsigned short)inFirstByte << 8) | buffer;
			return ePDFSuccess;
		}
		else
			return ePDFFailure;
	}
	else
	{
		outOperator = inFirstByte;
		return ePDFSuccess;
	}
}

EPDFStatusCode CFFPrimitiveReader::ReadDictOperand(Byte inFirstByte,DictOperand& outOperand)
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
		return ePDFFailure; // not an operand
}

/*
   Source File : CFFPrimitiveWriter.cpp


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
#include "CFFPrimitiveWriter.h"
#include <math.h>

CFFPrimitiveWriter::CFFPrimitiveWriter(IByteWriter* inCFFOutput)
{
	SetStream(inCFFOutput);
}

CFFPrimitiveWriter::~CFFPrimitiveWriter(void)
{
}

void CFFPrimitiveWriter::SetStream(IByteWriter* inCFFOutput)
{
	mCFFOutput = inCFFOutput;
	if(inCFFOutput)
	{
		mCurrentOffsize = 1;
		mInternalState = ePDFSuccess;
	}
	else
	{
		mInternalState = ePDFFailure;
	}	
}

EPDFStatusCode CFFPrimitiveWriter::GetInternalState()
{
	return mInternalState;
}

EPDFStatusCode CFFPrimitiveWriter::WriteByte(Byte inValue)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	EPDFStatusCode status = (mCFFOutput->Write(&inValue,1) == 1 ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	return status;		
}

EPDFStatusCode CFFPrimitiveWriter::Write(const Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(ePDFFailure == mInternalState)
		return ePDFFailure;

	EPDFStatusCode status = (mCFFOutput->Write(inBuffer,inBufferSize) == inBufferSize ? ePDFSuccess : ePDFFailure);

	if(ePDFFailure == status)
		mInternalState = ePDFFailure;
	return status;	
}

EPDFStatusCode CFFPrimitiveWriter::WriteCard8(Byte inValue)
{
	return WriteByte(inValue);
}

EPDFStatusCode CFFPrimitiveWriter::WriteCard16(unsigned short inValue)
{
	Byte byte1 = (inValue>>8) & 0xff;
	Byte byte2 = inValue & 0xff;
	
	if(WriteByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;	
}

void CFFPrimitiveWriter::SetOffSize(Byte inOffSize)
{
	mCurrentOffsize = inOffSize;
}

EPDFStatusCode CFFPrimitiveWriter::WriteOffset(unsigned long inValue)
{
	EPDFStatusCode status = ePDFFailure;

	switch(mCurrentOffsize)
	{
		case 1:
			status = WriteCard8((Byte)inValue);
			break;
		case 2:
			status = WriteCard16((unsigned short)inValue);
			break;
		case 3:
			status = Write3ByteUnsigned(inValue);
			break;
		case 4:
			status = Write4ByteUnsigned(inValue);
			break;

	}
	return status;	
}

EPDFStatusCode CFFPrimitiveWriter::Write3ByteUnsigned(unsigned long inValue)
{
	Byte byte1 = (inValue>>16) & 0xff;
	Byte byte2 = (inValue>>8) & 0xff;
	Byte byte3 = inValue & 0xff;
	
	if(WriteByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte3) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;
}

EPDFStatusCode CFFPrimitiveWriter::Write4ByteUnsigned(unsigned long inValue)
{
	Byte byte1 = (inValue>>24) & 0xff;
	Byte byte2 = (inValue>>16) & 0xff;
	Byte byte3 = (inValue>>8) & 0xff;
	Byte byte4 = inValue & 0xff;
	
	if(WriteByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte3) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;
}



EPDFStatusCode CFFPrimitiveWriter::WriteOffSize(Byte inValue)
{
	return WriteCard8(inValue);
}

EPDFStatusCode CFFPrimitiveWriter::WriteSID(unsigned short inValue)
{
	return WriteCard16(inValue);
}

EPDFStatusCode CFFPrimitiveWriter::WriteDictOperator(unsigned short inOperator)
{
	if(((inOperator >> 8)  & 0xff) == 12)
		return WriteCard16(inOperator);
	else
		return WriteCard8((Byte)(inOperator & 0xff));
}

EPDFStatusCode CFFPrimitiveWriter::WriteDictOperand(const DictOperand& inOperand)
{	
	if(inOperand.IsInteger)
		return WriteIntegerOperand(inOperand.IntegerValue);
	else
		return WriteRealOperand(inOperand.RealValue,inOperand.RealValueFractalEnd);
}

EPDFStatusCode CFFPrimitiveWriter::WriteDictItems(unsigned short inOperator,
											   const DictOperandList& inOperands)
{
	EPDFStatusCode status = ePDFSuccess;
	DictOperandList::const_iterator it = inOperands.begin();

	for(; it != inOperands.end() && ePDFSuccess == status; ++it)
		status = WriteDictOperand(*it);
	if(ePDFSuccess == status)
		status = WriteDictOperator(inOperator);

	return status;
}


EPDFStatusCode CFFPrimitiveWriter::WriteIntegerOperand(long inValue)
{
	if(-107 <= inValue && inValue <= 107)
	{
		return WriteByte((Byte)(inValue + 139));
	}
	else if(108 <= inValue && inValue <= 1131)
	{
		Byte byte0,byte1;

		inValue-=108;
		byte0 = ((inValue >> 8) & 0xff) + 247;
		byte1 = inValue & 0xff;

		if(WriteByte(byte0) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;
	}
	else if(-1131 <= inValue && inValue <= -108)
	{
		Byte byte0,byte1;

		inValue = -(inValue + 108);

		byte0 = ((inValue >> 8) & 0xff) + 251;
		byte1 = inValue & 0xff;

		if(WriteByte(byte0) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;
	}
	else if(-32768 <= inValue && inValue<= 32767)
	{
		Byte byte1,byte2;

		byte1 = (inValue >> 8) & 0xff;
		byte2 = inValue & 0xff;

		if(WriteByte(28) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte2) != ePDFSuccess)
			return ePDFFailure;
	}
	else //  -2^31 <= inValue <= 2^31 - 1
	{
		return Write5ByteDictInteger(inValue);
	}
	return ePDFSuccess;
}

EPDFStatusCode CFFPrimitiveWriter::Write5ByteDictInteger(long inValue)
{
	Byte byte1,byte2,byte3,byte4;

	byte1 = (inValue >> 24) & 0xff;
	byte2 = (inValue >> 16)& 0xff;
	byte3 = (inValue >> 8) & 0xff;
	byte4 = inValue & 0xff;

	if(WriteByte(29) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte1) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte2) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte3) != ePDFSuccess)
		return ePDFFailure;

	if(WriteByte(byte4) != ePDFSuccess)
		return ePDFFailure;

	return ePDFSuccess;
}


EPDFStatusCode CFFPrimitiveWriter::WriteRealOperand(double inValue,long inFractalLength)
{
	// first, calculate the proper formatting

	bool minusSign = inValue < 0;
	bool minusExponent = false;
	bool plusExponent = false;
	unsigned short exponentSize = 0;
	
	if(minusSign)
		inValue = -inValue;

	double integerValue = floor(inValue);
	double fractalValue = inValue - integerValue;

	if(0 == integerValue)
	{
		if(fractalValue <= 0.001) // bother only if < 0.001
		{
			minusExponent = true;
			while(fractalValue < 0.1)
			{
				++exponentSize;
				fractalValue = fractalValue * 10;
			}
		}
	}
	else if(0 == fractalValue)
	{
		if(long(integerValue) % 1000 == 0 && integerValue >= 1000) // bother only if larger than 1000
		{
			plusExponent = true;
			while(long(integerValue) % 10 == 0)
			{
				++exponentSize;
				integerValue = integerValue / 10;
			}
		}
	}

	// now let's get to work
	if(WriteByte(30) != ePDFSuccess)
		return ePDFFailure;
	
	// first, take care of minus sign
	Byte buffer = minusSign ? 0xe0 : 0;
	bool usedFirst = minusSign;

	// Integer part
	if(integerValue != 0)
		if(WriteIntegerOfReal(integerValue,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;

	// Fractal part (if there was an integer or not)
	if(fractalValue != 0)
	{
		if(0 == integerValue)
		{
			if(SetOrWriteNibble(0,buffer,usedFirst) != ePDFSuccess)
				return ePDFFailure;
		}

		if(SetOrWriteNibble(0xa,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;


		while(fractalValue != 0 && inFractalLength > 0)
		{
			if(SetOrWriteNibble((Byte)floor(fractalValue * 10),buffer,usedFirst) != ePDFSuccess)
				return ePDFFailure;
			fractalValue = fractalValue * 10 - floor(fractalValue * 10);
			--inFractalLength;
		}
	}

	// now, if there's any exponent, write it
	if(minusExponent)
	{
		if(SetOrWriteNibble(0xc,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;
		if(WriteIntegerOfReal(exponentSize,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;
	}
	if(plusExponent)
	{
		if(SetOrWriteNibble(0xb,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;
		if(WriteIntegerOfReal(exponentSize,buffer,usedFirst) != ePDFSuccess)
			return ePDFFailure;
	}

	// final f or ff
	if(usedFirst)
		return SetOrWriteNibble(0xf,buffer,usedFirst);
	else
		return WriteByte(0xff);

}

EPDFStatusCode CFFPrimitiveWriter::SetOrWriteNibble(Byte inValue,Byte& ioBuffer,bool& ioUsedFirst)
{
	EPDFStatusCode status = ePDFSuccess;
	if(ioUsedFirst)
	{
		ioBuffer|= inValue;
		status = WriteByte(ioBuffer);
		ioBuffer = 0;
		ioUsedFirst = false;
	}
	else
	{
		ioBuffer = (inValue << 4) & 0xf0;
		ioUsedFirst = true;
	}
	return status;
}

EPDFStatusCode CFFPrimitiveWriter::WriteIntegerOfReal(double inIntegerValue,
												   Byte& ioBuffer,
												   bool& ioUsedFirst)
{
	if(0 == inIntegerValue)
		return ePDFSuccess;

	EPDFStatusCode status = WriteIntegerOfReal(floor(inIntegerValue/10),ioBuffer,ioUsedFirst);
	if(status != ePDFSuccess)
		return ePDFFailure;

	return SetOrWriteNibble((Byte)(long(inIntegerValue) % 10),ioBuffer,ioUsedFirst);
}

Byte BytesPad5[5] = {'0','0','0','0','0'};
EPDFStatusCode CFFPrimitiveWriter::Pad5Bytes()
{
	return Write(BytesPad5,5);	
}

Byte BytePad[1] = {'0'};
EPDFStatusCode CFFPrimitiveWriter::PadNBytes(unsigned short inBytesToPad)
{
	EPDFStatusCode status = ePDFSuccess;

	for(unsigned short i=0;i<inBytesToPad && ePDFSuccess == status;++i)
		Write(BytePad,1);
	return status;
}
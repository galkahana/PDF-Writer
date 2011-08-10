/*
   Source File : Type2CharStringWriter.cpp


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
#include "Type2CharStringWriter.h"
#include "IByteWriter.h"

Type2CharStringWriter::Type2CharStringWriter(IByteWriter* inTargetStream)
{
	mTargetStream = inTargetStream;
}

Type2CharStringWriter::~Type2CharStringWriter(void)
{
}

void Type2CharStringWriter::Assign(IByteWriter* inTargetStream)
{
	mTargetStream = inTargetStream;
}

EPDFStatusCode Type2CharStringWriter::WriteHintMask(unsigned long inMask,unsigned long inMaskSize)
{
	unsigned long maskByteSize = inMaskSize/8 + (inMaskSize % 8 != 0 ? 1:0);	
	
	EPDFStatusCode status = WriteOperator(19);
	if(status != ePDFSuccess)
		return status;

	return WriteMaskBytes(inMask,maskByteSize);
}

EPDFStatusCode Type2CharStringWriter::WriteMaskBytes(unsigned long inMask,unsigned long inMaskByteSize)
{
	EPDFStatusCode status;

	if(inMaskByteSize > 1)
	{
		status = WriteMaskBytes(inMask >> 1,inMaskByteSize - 1);
		if(status != ePDFSuccess)
			return status;
	}
	return WriteByte((Byte)(inMask & 0xff));
}

EPDFStatusCode Type2CharStringWriter::WriteByte(Byte inValue)
{
	return (mTargetStream->Write(&inValue,1) == 1 ? ePDFSuccess:ePDFFailure);
}

EPDFStatusCode Type2CharStringWriter::WriteIntegerOperand(long inOperand)
{
	long value = inOperand;

	if(-107 <= value && value <= 107)
	{
		return WriteByte((Byte)(value + 139));
	}
	else if(108 <= value && value <= 1131)
	{
		Byte byte0,byte1;

		value-=108;
		byte0 = ((value >> 8) & 0xff) + 247;
		byte1 = value & 0xff;

		if(WriteByte(byte0) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;
		return ePDFSuccess;
	}
	else if(-1131 <= value && value <= -108)
	{
		Byte byte0,byte1;

		value = -(value + 108);

		byte0 = ((value >> 8) & 0xff) + 251;
		byte1 = value & 0xff;

		if(WriteByte(byte0) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;
		return ePDFSuccess;
	}
	else if(-32768 <= value && value<= 32767)
	{
		Byte byte1,byte2;

		byte1 = (value >> 8) & 0xff;
		byte2 = value & 0xff;

		if(WriteByte(28) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte1) != ePDFSuccess)
			return ePDFFailure;

		if(WriteByte(byte2) != ePDFSuccess)
			return ePDFFailure;
		return ePDFSuccess;
	}
	else
		return ePDFFailure;

}

EPDFStatusCode Type2CharStringWriter::WriteOperator(unsigned short inOperatorCode)
{
	if((inOperatorCode & 0xff00) == 0x0c00)
	{
		if(WriteByte(0xc0) != ePDFSuccess)
			return ePDFFailure;
	}
	return WriteByte(Byte(inOperatorCode & 0xff));	
}


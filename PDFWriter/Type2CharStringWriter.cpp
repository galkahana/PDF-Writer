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

EStatusCode Type2CharStringWriter::WriteHintMask(unsigned long inMask,unsigned long inMaskSize)
{
	unsigned long maskByteSize = inMaskSize/8 + (inMaskSize % 8 != 0 ? 1:0);	

	return WriteMaskBytes(inMask,maskByteSize);
}

EStatusCode Type2CharStringWriter::WriteMaskBytes(unsigned long inMask,unsigned long inMaskByteSize)
{
	EStatusCode status;

	if(inMaskByteSize > 1)
	{
		status = WriteMaskBytes(inMask >> 1,inMaskByteSize - 1);
		if(status != eSuccess)
			return status;
	}
	return WriteByte((Byte)(inMask & 0xff));
}

EStatusCode Type2CharStringWriter::WriteByte(Byte inValue)
{
	return (mTargetStream->Write(&inValue,1) == 1 ? eSuccess:eFailure);
}

EStatusCode Type2CharStringWriter::WriteIntegerOperand(long inOperand)
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

		if(WriteByte(byte0) != eSuccess)
			return eFailure;

		if(WriteByte(byte1) != eSuccess)
			return eFailure;
		return eSuccess;
	}
	else if(-1131 <= value && value <= -108)
	{
		Byte byte0,byte1;

		value = -(value + 108);

		byte0 = ((value >> 8) & 0xff) + 251;
		byte1 = value & 0xff;

		if(WriteByte(byte0) != eSuccess)
			return eFailure;

		if(WriteByte(byte1) != eSuccess)
			return eFailure;
		return eSuccess;
	}
	else if(-32768 <= value && value<= 32767)
	{
		Byte byte1,byte2;

		byte1 = (value >> 8) & 0xff;
		byte2 = value & 0xff;

		if(WriteByte(28) != eSuccess)
			return eFailure;

		if(WriteByte(byte1) != eSuccess)
			return eFailure;

		if(WriteByte(byte2) != eSuccess)
			return eFailure;
		return eSuccess;
	}
	else
		return eFailure;

}

EStatusCode Type2CharStringWriter::WriteOperator(unsigned short inOperatorCode)
{
	if((inOperatorCode & 0xff00) == 0x0c00)
	{
		if(WriteByte(0xc0) != eSuccess)
			return eFailure;
	}
	return WriteByte(Byte(inOperatorCode & 0xff));	
}


#pragma once
#include "EStatusCode.h"
#include "IOBasicTypes.h"

using namespace IOBasicTypes;

class IByteWriter;

class Type2CharStringWriter
{
public:
	Type2CharStringWriter(IByteWriter* inTargetStream);
	~Type2CharStringWriter(void);

	void Assign(IByteWriter* inTargetStream);

	EStatusCode WriteHintMask(unsigned long inMask,unsigned long inMaskSize);
	EStatusCode WriteIntegerOperand(long inOperand);
	EStatusCode WriteOperator(unsigned short inOperatorCode);


private:

	IByteWriter* mTargetStream;

	EStatusCode WriteMaskBytes(unsigned long inMask,unsigned long inMaskByteSize);
	EStatusCode WriteByte(Byte inValue);
};

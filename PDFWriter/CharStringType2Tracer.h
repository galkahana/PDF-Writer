#pragma once

#include "EStatusCode.h"
#include "IType2InterpreterImplementation.h"
#include "PrimitiveObjectsWriter.h"

class CFFFileInput;
class IByteWriter;

class CharStringType2Tracer : public IType2InterpreterImplementation
{
public:
	CharStringType2Tracer(void);
	~CharStringType2Tracer(void);

	EStatusCode TraceGlyphProgram(unsigned short inFontIndex, unsigned short inGlyphIndex, CFFFileInput* inCFFFileInput, IByteWriter* inWriter);

	// IType2InterpreterImplementation implementation
	virtual EStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString);	
	virtual EStatusCode Type2InterpretNumber(const CharStringOperand& inOperand);
	virtual EStatusCode Type2Hstem(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vstem(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vmoveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Rlineto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hlineto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vlineto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2RRCurveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Return(const CharStringOperandList& inOperandList) ;
	virtual EStatusCode Type2Endchar(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hstemhm(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter);
	virtual EStatusCode Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter);
	virtual EStatusCode Type2Rmoveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hmoveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vstemhm(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Rcurveline(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Rlinecurve(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vvcurveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hvcurveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hhcurveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Vhcurveto(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hflex(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Hflex1(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Flex(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Flex1(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2And(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Or(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Not(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Abs(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Add(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Sub(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Div(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Neg(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Eq(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Drop(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Put(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Get(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Ifelse(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Random(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Mul(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Sqrt(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Dup(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Exch(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Index(const CharStringOperandList& inOperandList);
	virtual EStatusCode Type2Roll(const CharStringOperandList& inOperandList);
	virtual CharString* GetLocalSubr(long inSubrIndex);
	virtual CharString* GetGlobalSubr(long inSubrIndex);

private:
	IByteWriter* mWriter;
	CFFFileInput* mHelper;
	PrimitiveObjectsWriter mPrimitiveWriter;
	unsigned short mStemsCount;

	void WriteStemMask(Byte* inProgramCounter);
};

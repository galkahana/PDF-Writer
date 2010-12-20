#pragma once
#include "EStatusCode.h"
#include "IType1InterpreterImplementation.h"
#include "PrimitiveObjectsWriter.h"
#include <string>

class Type1Input;
class IByteWriter;

using namespace std;

class CharStringType1Tracer: public IType1InterpreterImplementation
{
public:
	CharStringType1Tracer(void);
	~CharStringType1Tracer(void);

	EStatusCode TraceGlyphProgram(Byte inGlyphIndex, Type1Input* inType1Input, IByteWriter* inWriter);
	EStatusCode TraceGlyphProgram(const string& inGlyphName, Type1Input* inType1Input, IByteWriter* inWriter);

	// IType1InterpreterImplementation
	virtual EStatusCode Type1Hstem(const LongList& inOperandList);
	virtual EStatusCode Type1Vstem(const LongList& inOperandList);
	virtual EStatusCode Type1VMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1RLineto(const LongList& inOperandList);
	virtual EStatusCode Type1HLineto(const LongList& inOperandList);
	virtual EStatusCode Type1VLineto(const LongList& inOperandList);
	virtual EStatusCode Type1RRCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1ClosePath(const LongList& inOperandList);
	virtual Type1CharString* GetSubr(long inSubrIndex);
	virtual EStatusCode Type1Return(const LongList& inOperandList);
	virtual EStatusCode Type1Hsbw(const LongList& inOperandList);
	virtual EStatusCode Type1Endchar(const LongList& inOperandList);
	virtual EStatusCode Type1RMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1HMoveto(const LongList& inOperandList);
	virtual EStatusCode Type1VHCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1HVCurveto(const LongList& inOperandList);
	virtual EStatusCode Type1DotSection(const LongList& inOperandList);
	virtual EStatusCode Type1VStem3(const LongList& inOperandList);
	virtual EStatusCode Type1HStem3(const LongList& inOperandList);
	virtual EStatusCode Type1Seac(const LongList& inOperandList);
	virtual EStatusCode Type1Sbw(const LongList& inOperandList);
	virtual EStatusCode Type1Div(const LongList& inOperandList);
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
	virtual EStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack);
	virtual EStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack);
	virtual EStatusCode Type1SetCurrentPoint(const LongList& inOperandList);
	virtual EStatusCode Type1InterpretNumber(long inOperand);
	virtual unsigned long GetLenIV();

private:
	IByteWriter* mWriter;
	Type1Input* mHelper;
	PrimitiveObjectsWriter mPrimitiveWriter;

};

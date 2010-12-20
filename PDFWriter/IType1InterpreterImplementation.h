#pragma once

#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <list>

using namespace std;
using namespace IOBasicTypes;

struct Type1CharString
{
	Byte* Code;
	int CodeLength;
};

typedef list<long> LongList;

class IType1InterpreterImplementation
{
public:

	virtual EStatusCode Type1Hstem(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Vstem(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1VMoveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1RLineto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1HLineto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1VLineto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1RRCurveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1ClosePath(const LongList& inOperandList) = 0;
	virtual Type1CharString* GetSubr(long inSubrIndex) = 0;
	virtual EStatusCode Type1Return(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Hsbw(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Endchar(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1RMoveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1HMoveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1VHCurveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1HVCurveto(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1DotSection(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1VStem3(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1HStem3(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Seac(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Sbw(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1Div(const LongList& inOperandList) = 0;
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex) = 0;
	virtual EStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack) = 0;
	virtual EStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack) = 0;
	virtual EStatusCode Type1SetCurrentPoint(const LongList& inOperandList) = 0;
	virtual EStatusCode Type1InterpretNumber(long inOperand) = 0;
	virtual unsigned long GetLenIV() = 0;

};

class Type1InterpreterImplementationAdapter : public IType1InterpreterImplementation
{
public:

	virtual EStatusCode Type1Hstem(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Vstem(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1VMoveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1RLineto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1HLineto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1VLineto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1RRCurveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1ClosePath(const LongList& inOperandList) {return eSuccess;}
	virtual Type1CharString* GetSubr(long inSubrIndex) {return NULL;}
	virtual EStatusCode Type1Return(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Hsbw(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Endchar(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1RMoveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1HMoveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1VHCurveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1HVCurveto(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1DotSection(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1VStem3(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1HStem3(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Seac(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Sbw(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1Div(const LongList& inOperandList) {return eSuccess;}
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex) {return false;}
	virtual EStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack) {return eSuccess;}
	virtual EStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack) {return eSuccess;}
	virtual EStatusCode Type1SetCurrentPoint(const LongList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type1InterpretNumber(long inOperand) {return eSuccess;}
	virtual unsigned long GetLenIV() {return 4;}
};
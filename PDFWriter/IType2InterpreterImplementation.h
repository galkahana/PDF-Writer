#pragma once

#include "EStatusCode.h"
#include "CharStringDefinitions.h"

class IType2InterpreterImplementation
{
public:
	virtual EStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString) = 0;	

	// events in the code
	virtual EStatusCode Type2InterpretNumber(const CharStringOperand& inOperand) = 0;
	virtual EStatusCode Type2Hstem(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Vstem(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Vmoveto(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Rlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Hlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Vlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2RRCurveto(const CharStringOperandList& inOperandList) = 0;
	virtual EStatusCode Type2Return(const CharStringOperandList& inOperandList) =0;
	virtual EStatusCode Type2Endchar(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hstemhm(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)=0;
	virtual EStatusCode Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)=0;
	virtual EStatusCode Type2Rmoveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hmoveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Vstemhm(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Rcurveline(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Rlinecurve(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Vvcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hvcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hhcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Vhcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hflex(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Hflex1(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Flex(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Flex1(const CharStringOperandList& inOperandList)=0;
	
	virtual EStatusCode Type2And(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Or(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Not(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Abs(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Add(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Sub(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Div(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Neg(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Eq(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Drop(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Put(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Get(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Ifelse(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Random(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Mul(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Sqrt(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Dup(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Exch(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Index(const CharStringOperandList& inOperandList)=0;
	virtual EStatusCode Type2Roll(const CharStringOperandList& inOperandList)=0;
	
	
	
	
	
	
	
	virtual CharString* GetLocalSubr(long inSubrIndex) = 0; // you should bias the index !!
	virtual CharString* GetGlobalSubr(long inSubrIndex) = 0;// you should bias the index !!
	
};

class Type2InterpreterImplementationAdapter : public IType2InterpreterImplementation
{
public:
	virtual EStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString){return eFailure;}	

	virtual EStatusCode Type2InterpretNumber(const CharStringOperand& inOperand) {return eSuccess;};
	virtual EStatusCode Type2Hstem(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Vstem(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Vmoveto(const CharStringOperandList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type2Rlineto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hlineto(const CharStringOperandList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type2Vlineto(const CharStringOperandList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type2RRCurveto(const CharStringOperandList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type2Return(const CharStringOperandList& inOperandList) {return eSuccess;}
	virtual EStatusCode Type2Endchar(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hstemhm(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter){return eSuccess;}
	virtual EStatusCode Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter){return eSuccess;}
	virtual EStatusCode Type2Rmoveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hmoveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Vstemhm(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Rcurveline(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Rlinecurve(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Vvcurveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hvcurveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hhcurveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Vhcurveto(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hflex(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Hflex1(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Flex(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Flex1(const CharStringOperandList& inOperandList){return eSuccess;}

	virtual EStatusCode Type2And(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Or(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Not(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Abs(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Add(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Sub(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Div(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Neg(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Eq(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Drop(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Put(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Get(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Ifelse(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Random(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Mul(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Sqrt(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Dup(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Exch(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Index(const CharStringOperandList& inOperandList){return eSuccess;}
	virtual EStatusCode Type2Roll(const CharStringOperandList& inOperandList){return eSuccess;}	
	
	virtual CharString* GetLocalSubr(long inSubrIndex) {return NULL;}
	virtual CharString* GetGlobalSubr(long inSubrIndex){return NULL;}
};

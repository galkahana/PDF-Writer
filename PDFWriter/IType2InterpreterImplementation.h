/*
   Source File : IType2InterpreterImplementation.h


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
#pragma once

#include "EPDFStatusCode.h"
#include "CharStringDefinitions.h"

class IType2InterpreterImplementation
{
public:
	virtual EPDFStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString) = 0;	

	// events in the code
	virtual EPDFStatusCode Type2InterpretNumber(const CharStringOperand& inOperand) = 0;
	virtual EPDFStatusCode Type2Hstem(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Vstem(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Vmoveto(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Rlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Hlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Vlineto(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2RRCurveto(const CharStringOperandList& inOperandList) = 0;
	virtual EPDFStatusCode Type2Return(const CharStringOperandList& inOperandList) =0;
	virtual EPDFStatusCode Type2Endchar(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hstemhm(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)=0;
	virtual EPDFStatusCode Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)=0;
	virtual EPDFStatusCode Type2Rmoveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hmoveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Vstemhm(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Rcurveline(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Rlinecurve(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Vvcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hvcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hhcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Vhcurveto(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hflex(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Hflex1(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Flex(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Flex1(const CharStringOperandList& inOperandList)=0;
	
	virtual EPDFStatusCode Type2And(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Or(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Not(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Abs(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Add(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Sub(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Div(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Neg(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Eq(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Drop(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Put(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Get(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Ifelse(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Random(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Mul(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Sqrt(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Dup(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Exch(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Index(const CharStringOperandList& inOperandList)=0;
	virtual EPDFStatusCode Type2Roll(const CharStringOperandList& inOperandList)=0;
	
	virtual CharString* GetLocalSubr(long inSubrIndex) = 0; // you should bias the index !!
	virtual CharString* GetGlobalSubr(long inSubrIndex) = 0;// you should bias the index !!
	
};

class Type2InterpreterImplementationAdapter : public IType2InterpreterImplementation
{
public:
	virtual EPDFStatusCode ReadCharString(LongFilePositionType inCharStringStart,
							   LongFilePositionType inCharStringEnd,
							   Byte** outCharString){return ePDFFailure;}	

	virtual EPDFStatusCode Type2InterpretNumber(const CharStringOperand& inOperand) {return ePDFSuccess;};
	virtual EPDFStatusCode Type2Hstem(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vstem(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vmoveto(const CharStringOperandList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type2Rlineto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hlineto(const CharStringOperandList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vlineto(const CharStringOperandList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type2RRCurveto(const CharStringOperandList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type2Return(const CharStringOperandList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type2Endchar(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hstemhm(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Rmoveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hmoveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vstemhm(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Rcurveline(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Rlinecurve(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vvcurveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hvcurveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hhcurveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Vhcurveto(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hflex(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Hflex1(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Flex(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Flex1(const CharStringOperandList& inOperandList){return ePDFSuccess;}

	virtual EPDFStatusCode Type2And(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Or(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Not(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Abs(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Add(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Sub(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Div(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Neg(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Eq(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Drop(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Put(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Get(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Ifelse(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Random(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Mul(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Sqrt(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Dup(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Exch(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Index(const CharStringOperandList& inOperandList){return ePDFSuccess;}
	virtual EPDFStatusCode Type2Roll(const CharStringOperandList& inOperandList){return ePDFSuccess;}	
	
	virtual CharString* GetLocalSubr(long inSubrIndex) {return NULL;}
	virtual CharString* GetGlobalSubr(long inSubrIndex){return NULL;}
};

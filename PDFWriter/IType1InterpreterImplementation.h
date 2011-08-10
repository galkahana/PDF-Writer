/*
   Source File : IType1InterpreterImplementation.h


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

	virtual EPDFStatusCode Type1Hstem(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Vstem(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1VMoveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1RLineto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1HLineto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1VLineto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1RRCurveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1ClosePath(const LongList& inOperandList) = 0;
	virtual Type1CharString* GetSubr(long inSubrIndex) = 0;
	virtual EPDFStatusCode Type1Return(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Hsbw(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Endchar(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1RMoveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1HMoveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1VHCurveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1HVCurveto(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1DotSection(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1VStem3(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1HStem3(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Seac(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Sbw(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1Div(const LongList& inOperandList) = 0;
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex) = 0;
	virtual EPDFStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack) = 0;
	virtual EPDFStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack) = 0;
	virtual EPDFStatusCode Type1SetCurrentPoint(const LongList& inOperandList) = 0;
	virtual EPDFStatusCode Type1InterpretNumber(long inOperand) = 0;
	virtual unsigned long GetLenIV() = 0;

};

class Type1InterpreterImplementationAdapter : public IType1InterpreterImplementation
{
public:

	virtual EPDFStatusCode Type1Hstem(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Vstem(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1VMoveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1RLineto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1HLineto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1VLineto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1RRCurveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1ClosePath(const LongList& inOperandList) {return ePDFSuccess;}
	virtual Type1CharString* GetSubr(long inSubrIndex) {return NULL;}
	virtual EPDFStatusCode Type1Return(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Hsbw(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Endchar(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1RMoveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1HMoveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1VHCurveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1HVCurveto(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1DotSection(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1VStem3(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1HStem3(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Seac(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Sbw(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Div(const LongList& inOperandList) {return ePDFSuccess;}
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex) {return false;}
	virtual EPDFStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1SetCurrentPoint(const LongList& inOperandList) {return ePDFSuccess;}
	virtual EPDFStatusCode Type1InterpretNumber(long inOperand) {return ePDFSuccess;}
	virtual unsigned long GetLenIV() {return 4;}
};
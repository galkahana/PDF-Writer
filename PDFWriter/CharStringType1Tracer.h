/*
   Source File : CharStringType1Tracer.h


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

	EPDFStatusCode TraceGlyphProgram(Byte inGlyphIndex, Type1Input* inType1Input, IByteWriter* inWriter);
	EPDFStatusCode TraceGlyphProgram(const string& inGlyphName, Type1Input* inType1Input, IByteWriter* inWriter);

	// IType1InterpreterImplementation
	virtual EPDFStatusCode Type1Hstem(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Vstem(const LongList& inOperandList);
	virtual EPDFStatusCode Type1VMoveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1RLineto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1HLineto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1VLineto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1RRCurveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1ClosePath(const LongList& inOperandList);
	virtual Type1CharString* GetSubr(long inSubrIndex);
	virtual EPDFStatusCode Type1Return(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Hsbw(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Endchar(const LongList& inOperandList);
	virtual EPDFStatusCode Type1RMoveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1HMoveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1VHCurveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1HVCurveto(const LongList& inOperandList);
	virtual EPDFStatusCode Type1DotSection(const LongList& inOperandList);
	virtual EPDFStatusCode Type1VStem3(const LongList& inOperandList);
	virtual EPDFStatusCode Type1HStem3(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Seac(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Sbw(const LongList& inOperandList);
	virtual EPDFStatusCode Type1Div(const LongList& inOperandList);
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
	virtual EPDFStatusCode CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack);
	virtual EPDFStatusCode Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack);
	virtual EPDFStatusCode Type1SetCurrentPoint(const LongList& inOperandList);
	virtual EPDFStatusCode Type1InterpretNumber(long inOperand);
	virtual unsigned long GetLenIV();

private:
	IByteWriter* mWriter;
	Type1Input* mHelper;
	PrimitiveObjectsWriter mPrimitiveWriter;

};

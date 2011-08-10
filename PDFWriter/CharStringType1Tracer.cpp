/*
   Source File : CharStringType1Tracer.cpp


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
#include "CharStringType1Tracer.h"
#include "CharStringType1Interpreter.h"
#include "Type1Input.h"
#include "Trace.h"

CharStringType1Tracer::CharStringType1Tracer(void)
{
}

CharStringType1Tracer::~CharStringType1Tracer(void)
{
}

/*
EPDFStatusCode CharStringType1Tracer::TraceGlyphProgram(Byte inGlyphIndex, Type1Input* inType1Input, IByteWriter* inWriter)
{
	CharStringType1Interpreter interpreter;

	mWriter = inWriter;
	mHelper = inType1Input;
	mPrimitiveWriter.SetStreamForWriting(inWriter);

	Type1CharString* charString = inType1Input->GetGlyphCharString(inGlyphIndex);
	if(!charString)
	{
		TRACE_LOG("CharStringType1Tracer::TraceGlyphProgram, Exception, cannot find glyph index");
		return ePDFFailure;
	}

	return interpreter.Intepret(*charString,this);
}*/

EPDFStatusCode CharStringType1Tracer::TraceGlyphProgram(const string& inGlyphName, Type1Input* inType1Input, IByteWriter* inWriter)
{
	CharStringType1Interpreter interpreter;

	mWriter = inWriter;
	mHelper = inType1Input;
	mPrimitiveWriter.SetStreamForWriting(inWriter);

	Type1CharString* charString = inType1Input->GetGlyphCharString(inGlyphName);
	if(!charString)
	{
		TRACE_LOG("CharStringType1Tracer::TraceGlyphProgram, Exception, cannot find glyph name");
		return ePDFFailure;
	}

	return interpreter.Intepret(*charString,this);
}

EPDFStatusCode CharStringType1Tracer::Type1Hstem(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hstem");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Vstem(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1VMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vmoveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1RLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1HLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1VLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1RRCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rrcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1ClosePath(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("closepath");
	return ePDFSuccess;
}

Type1CharString* CharStringType1Tracer::GetSubr(long inSubrIndex)
{
	mPrimitiveWriter.WriteKeyword("callsubr");
	return mHelper->GetSubr(inSubrIndex);
}

EPDFStatusCode CharStringType1Tracer::Type1Return(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("return");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Hsbw(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hsbw");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Endchar(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("endchar");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1RMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rmoveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1HMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hmoveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1VHCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vhcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1HVCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hvcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1DotSection(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("dotsection");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1VStem3(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem3");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1HStem3(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hstem3");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Seac(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("seac");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Sbw(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sbw");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Div(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("div");
	return ePDFSuccess;
}

bool CharStringType1Tracer::IsOtherSubrSupported(long inOtherSubrsIndex)
{
	mPrimitiveWriter.WriteKeyword("callothersubr");
	return false;
}

EPDFStatusCode CharStringType1Tracer::CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack)
{
	// not looking to get here, due to IsOtherSubrSupported returning false
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack)
{
	mPrimitiveWriter.WriteKeyword("pop");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1SetCurrentPoint(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("setcurrentpoint");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType1Tracer::Type1InterpretNumber(long inOperand)
{
	mPrimitiveWriter.WriteInteger(inOperand);
	return ePDFSuccess;
}

unsigned long CharStringType1Tracer::GetLenIV()
{
	return mHelper->GetLenIV();
}
/*
   Source File : CharStringType2Tracer.cpp


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
#include "CharStringType2Tracer.h"
#include "CFFFileInput.h"
#include "IByteWriter.h"
#include "CharStringType2Interpreter.h"
#include "Trace.h"
#include "BoxingBase.h"
#include "SafeBufferMacrosDefs.h"

CharStringType2Tracer::CharStringType2Tracer(void)
{
}

CharStringType2Tracer::~CharStringType2Tracer(void)
{
}


EPDFStatusCode CharStringType2Tracer::TraceGlyphProgram(unsigned short inFontIndex, 
													unsigned short inGlyphIndex, 
													CFFFileInput* inCFFFileInput, 
													IByteWriter* inWriter)
{
	CharStringType2Interpreter interpreter;
	EPDFStatusCode status = inCFFFileInput->PrepareForGlyphIntepretation(inFontIndex,inGlyphIndex);

	mWriter = inWriter;
	mHelper = inCFFFileInput;
	mPrimitiveWriter.SetStreamForWriting(inWriter);
	mStemsCount = 0;

	do
	{
		if(status != ePDFSuccess)
		{
			TRACE_LOG("CharStringType2Tracer::Trace, Exception, cannot prepare for glyph interpretation");
			break;
		}
		
		CharString* charString = inCFFFileInput->GetGlyphCharString(inFontIndex,inGlyphIndex);
		if(!charString)
		{
			TRACE_LOG("CharStringType2Tracer::Trace, Exception, cannot find glyph index");
			break;
		}

		status = interpreter.Intepret(*charString,this);

	}while(false);
	return status;
}

EPDFStatusCode CharStringType2Tracer::ReadCharString(LongFilePositionType inCharStringStart,
						   LongFilePositionType inCharStringEnd,
						   Byte** outCharString)
{
	return mHelper->ReadCharString(inCharStringStart,inCharStringEnd,outCharString);
}

EPDFStatusCode CharStringType2Tracer::Type2InterpretNumber(const CharStringOperand& inOperand)
{
	if(inOperand.IsInteger)
		mPrimitiveWriter.WriteInteger(inOperand.IntegerValue);
	else
		mPrimitiveWriter.WriteDouble(inOperand.RealValue);
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hstem(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("hstem");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vstem(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("vstem");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Rlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vlineto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2RRCurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rrcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Return(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("return");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Endchar(const CharStringOperandList& inOperandList)
{
	// no need to call the CFFFileInput endchar here. that call is used for dependencies check alone
	// and provides for CFFFileInput own intepreter implementation.

	mPrimitiveWriter.WriteKeyword("endchar");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hstemhm(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("hstemhm");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	WriteStemMask(inProgramCounter);
	mPrimitiveWriter.WriteKeyword("hintmask");
	return ePDFSuccess;
}

void CharStringType2Tracer::WriteStemMask(Byte* inProgramCounter)
{
	unsigned short maskSize = mStemsCount/8 + (mStemsCount % 8 != 0 ? 1:0);
	char buffer[3];

	mWriter->Write((const Byte*)"(0x",1);
	for(unsigned short i=0;i<maskSize;++i)
	{
		SAFE_SPRINTF_1(buffer,3,"%X",inProgramCounter[i]);
		mWriter->Write((const Byte*)buffer,2);
	}

	mWriter->Write((const Byte*)")",1);
}

EPDFStatusCode CharStringType2Tracer::Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)
{
	WriteStemMask(inProgramCounter);
	mPrimitiveWriter.WriteKeyword("cntrmask");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Rmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rmoveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hmoveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vstemhm(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("vstemhm");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Rcurveline(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rcurveline");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Rlinecurve(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlinecurve");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vvcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vvcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hvcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hvcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hhcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hhcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Vhcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vhcurveto");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hflex(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hflex");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Hflex1(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hflex1");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Flex(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("flex");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Flex1(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("flex1");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2And(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("and");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Or(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("or");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Not(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("not");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Abs(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("abs");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Add(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("add");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Sub(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sub");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Div(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("div");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Neg(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("neg");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Eq(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("eq");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Drop(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("drop");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Put(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("put");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Get(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("get");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Ifelse(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("ifelse");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Random(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("random");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Mul(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("mul");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Sqrt(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sqrt");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Dup(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("dup");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Exch(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("exch");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Index(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("index");
	return ePDFSuccess;
}

EPDFStatusCode CharStringType2Tracer::Type2Roll(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("roll");
	return ePDFSuccess;
}

CharString* CharStringType2Tracer::GetLocalSubr(long inSubrIndex)
{
	mPrimitiveWriter.WriteKeyword("callsubr");

	return mHelper->GetLocalSubr(inSubrIndex);
}

CharString* CharStringType2Tracer::GetGlobalSubr(long inSubrIndex)
{
	mPrimitiveWriter.WriteKeyword("callgsubr");

	return mHelper->GetGlobalSubr(inSubrIndex);
}

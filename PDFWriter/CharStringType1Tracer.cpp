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
EStatusCode CharStringType1Tracer::TraceGlyphProgram(Byte inGlyphIndex, Type1Input* inType1Input, IByteWriter* inWriter)
{
	CharStringType1Interpreter interpreter;

	mWriter = inWriter;
	mHelper = inType1Input;
	mPrimitiveWriter.SetStreamForWriting(inWriter);

	Type1CharString* charString = inType1Input->GetGlyphCharString(inGlyphIndex);
	if(!charString)
	{
		TRACE_LOG("CharStringType1Tracer::TraceGlyphProgram, Exception, cannot find glyph index");
		return eFailure;
	}

	return interpreter.Intepret(*charString,this);
}*/

EStatusCode CharStringType1Tracer::TraceGlyphProgram(const string& inGlyphName, Type1Input* inType1Input, IByteWriter* inWriter)
{
	CharStringType1Interpreter interpreter;

	mWriter = inWriter;
	mHelper = inType1Input;
	mPrimitiveWriter.SetStreamForWriting(inWriter);

	Type1CharString* charString = inType1Input->GetGlyphCharString(inGlyphName);
	if(!charString)
	{
		TRACE_LOG("CharStringType1Tracer::TraceGlyphProgram, Exception, cannot find glyph name");
		return eFailure;
	}

	return interpreter.Intepret(*charString,this);
}

EStatusCode CharStringType1Tracer::Type1Hstem(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hstem");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Vstem(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1VMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vmoveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1RLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlineto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1HLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hlineto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1VLineto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vlineto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1RRCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rrcurveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1ClosePath(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("closepath");
	return eSuccess;
}

Type1CharString* CharStringType1Tracer::GetSubr(long inSubrIndex)
{
	mPrimitiveWriter.WriteKeyword("callsubr");
	return mHelper->GetSubr(inSubrIndex);
}

EStatusCode CharStringType1Tracer::Type1Return(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("return");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Hsbw(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hsbw");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Endchar(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("endchar");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1RMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rmoveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1HMoveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hmoveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1VHCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vhcurveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1HVCurveto(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hvcurveto");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1DotSection(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("dotsection");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1VStem3(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem3");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1HStem3(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hstem3");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Seac(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("seac");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Sbw(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sbw");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Div(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("div");
	return eSuccess;
}

bool CharStringType1Tracer::IsOtherSubrSupported(long inOtherSubrsIndex)
{
	mPrimitiveWriter.WriteKeyword("callothersubr");
	return false;
}

EStatusCode CharStringType1Tracer::CallOtherSubr(const LongList& inOperandList,LongList& outPostScriptOperandStack)
{
	// not looking to get here, due to IsOtherSubrSupported returning false
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1Pop(const LongList& inOperandList,const LongList& inPostScriptOperandStack)
{
	mPrimitiveWriter.WriteKeyword("pop");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1SetCurrentPoint(const LongList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("setcurrentpoint");
	return eSuccess;
}

EStatusCode CharStringType1Tracer::Type1InterpretNumber(long inOperand)
{
	mPrimitiveWriter.WriteInteger(inOperand);
	return eSuccess;
}

unsigned long CharStringType1Tracer::GetLenIV()
{
	return mHelper->GetLenIV();
}
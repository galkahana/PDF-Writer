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


EStatusCode CharStringType2Tracer::TraceGlyphProgram(unsigned short inFontIndex, 
													unsigned short inGlyphIndex, 
													CFFFileInput* inCFFFileInput, 
													IByteWriter* inWriter)
{
	CharStringType2Interpreter interpreter;
	EStatusCode status = inCFFFileInput->PrepareForGlyphIntepretation(inFontIndex,inGlyphIndex);

	mWriter = inWriter;
	mHelper = inCFFFileInput;
	mPrimitiveWriter.SetStreamForWriting(inWriter);
	mStemsCount = 0;

	do
	{
		if(status != eSuccess)
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

EStatusCode CharStringType2Tracer::ReadCharString(LongFilePositionType inCharStringStart,
						   LongFilePositionType inCharStringEnd,
						   Byte** outCharString)
{
	return mHelper->ReadCharString(inCharStringStart,inCharStringEnd,outCharString);
}

EStatusCode CharStringType2Tracer::Type2InterpretNumber(const CharStringOperand& inOperand)
{
	if(inOperand.IsInteger)
		mPrimitiveWriter.WriteInteger(inOperand.IntegerValue);
	else
		mPrimitiveWriter.WriteDouble(inOperand.RealValue);
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hstem(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("hstem");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vstem(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("vstem");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vstem");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlineto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hlineto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vlineto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vlineto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2RRCurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rrcurveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Return(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("return");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Endchar(const CharStringOperandList& inOperandList)
{
	// no need to call the CFFFileInput endchar here. that call is used for dependencies check alone
	// and provides for CFFFileInput own intepreter implementation.

	mPrimitiveWriter.WriteKeyword("endchar");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hstemhm(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("hstemhm");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hintmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	WriteStemMask(inProgramCounter);
	mPrimitiveWriter.WriteKeyword("hintmask");
	return eSuccess;
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

EStatusCode CharStringType2Tracer::Type2Cntrmask(const CharStringOperandList& inOperandList,Byte* inProgramCounter)
{
	WriteStemMask(inProgramCounter);
	mPrimitiveWriter.WriteKeyword("cntrmask");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rmoveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hmoveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hmoveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vstemhm(const CharStringOperandList& inOperandList)
{
	mStemsCount+= (unsigned short)(inOperandList.size() / 2);

	mPrimitiveWriter.WriteKeyword("vstemhm");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rcurveline(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rcurveline");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Rlinecurve(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("rlinecurve");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vvcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vvcurveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hvcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hvcurveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hhcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hhcurveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Vhcurveto(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("vhcurveto");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hflex(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hflex");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Hflex1(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("hflex1");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Flex(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("flex");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Flex1(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("flex1");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2And(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("and");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Or(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("or");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Not(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("not");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Abs(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("abs");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Add(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("add");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Sub(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sub");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Div(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("div");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Neg(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("neg");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Eq(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("eq");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Drop(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("drop");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Put(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("put");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Get(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("get");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Ifelse(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("ifelse");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Random(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("random");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Mul(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("mul");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Sqrt(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("sqrt");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Dup(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("dup");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Exch(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("exch");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Index(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("index");
	return eSuccess;
}

EStatusCode CharStringType2Tracer::Type2Roll(const CharStringOperandList& inOperandList)
{
	mPrimitiveWriter.WriteKeyword("roll");
	return eSuccess;
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

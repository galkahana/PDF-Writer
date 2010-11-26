#pragma once

#include "IType2InterpreterImplementation.h"
#include "CharStringDefinitions.h"

#include <vector>

using namespace std;

typedef vector<CharStringOperand> CharStringOperandVector;

class CharStringType2Interpreter
{
public:
	CharStringType2Interpreter(void);
	~CharStringType2Interpreter(void);

	EStatusCode Intepret(const CharString& inCharStringToIntepret, IType2InterpreterImplementation* inImplementationHelper);


private:
	CharStringOperandList mOperandStack;
	unsigned short mStemsCount;
	IType2InterpreterImplementation* mImplementationHelper;
	bool mGotEndChar;
	CharStringOperandVector mStorage;
	bool mCheckedWidth;


	EStatusCode ProcessCharString(Byte* inCharString,LongFilePositionType inCharStringLength);
	bool IsOperator(Byte* inProgramCounter);
	Byte* InterpretNumber(Byte* inProgramCounter);
	Byte* InterpretOperator(Byte* inProgramCounter,bool& outGotEndExecutionCommand);

	EStatusCode ClearNFromStack(unsigned short inCount);
	void ClearStack();
	void CheckWidth();

	Byte* InterpretHStem(Byte* inProgramCounter);
	Byte* InterpretVStem(Byte* inProgramCounter);
	Byte* InterpretVMoveto(Byte* inProgramCounter);
	Byte* InterpretRLineto(Byte* inProgramCounter);
	Byte* InterpretHLineto(Byte* inProgramCounter);
	Byte* InterpretVLineto(Byte* inProgramCounter);
	Byte* InterpretRRCurveto(Byte* inProgramCounter);
	Byte* InterpretCallSubr(Byte* inProgramCounter);
	Byte* InterpretReturn(Byte* inProgramCounter);
	Byte* InterpretEndChar(Byte* inProgramCounter);
	Byte* InterpretHStemHM(Byte* inProgramCounter);
	Byte* InterpretHintMask(Byte* inProgramCounter);
	Byte* InterpretCntrMask(Byte* inProgramCounter);
	Byte* InterpretRMoveto(Byte* inProgramCounter);
	Byte* InterpretHMoveto(Byte* inProgramCounter);
	Byte* InterpretVStemHM(Byte* inProgramCounter);
	Byte* InterpretRCurveLine(Byte* inProgramCounter);
	Byte* InterpretRLineCurve(Byte* inProgramCounter);
	Byte* InterpretVVCurveto(Byte* inProgramCounter);
	Byte* InterpretHHCurveto(Byte* inProgramCounter);
	Byte* InterpretCallGSubr(Byte* inProgramCounter);
	Byte* InterpretVHCurveto(Byte* inProgramCounter);
	Byte* InterpretHVCurveto(Byte* inProgramCounter);
	Byte* InterpretAnd(Byte* inProgramCounter);
	Byte* InterpretOr(Byte* inProgramCounter);
	Byte* InterpretNot(Byte* inProgramCounter);
	Byte* InterpretAbs(Byte* inProgramCounter);
	Byte* InterpretAdd(Byte* inProgramCounter);
	Byte* InterpretSub(Byte* inProgramCounter);
	Byte* InterpretDiv(Byte* inProgramCounter);
	Byte* InterpretNeg(Byte* inProgramCounter);
	Byte* InterpretEq(Byte* inProgramCounter);
	Byte* InterpretDrop(Byte* inProgramCounter);
	Byte* InterpretPut(Byte* inProgramCounter);
	Byte* InterpretGet(Byte* inProgramCounter);
	Byte* InterpretIfelse(Byte* inProgramCounter);
	Byte* InterpretRandom(Byte* inProgramCounter);
	Byte* InterpretMul(Byte* inProgramCounter);
	Byte* InterpretSqrt(Byte* inProgramCounter);
	Byte* InterpretDup(Byte* inProgramCounter);
	Byte* InterpretExch(Byte* inProgramCounter);
	Byte* InterpretIndex(Byte* inProgramCounter);
	Byte* InterpretRoll(Byte* inProgramCounter);
	Byte* InterpretHFlex(Byte* inProgramCounter);
	Byte* InterpretFlex(Byte* inProgramCounter);
	Byte* InterpretHFlex1(Byte* inProgramCounter);
	Byte* InterpretFlex1(Byte* inProgramCounter);
};

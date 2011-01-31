/*
   Source File : CharStringType1Interpreter.h


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
#include "IType1InterpreterImplementation.h"
#include "EStatusCode.h"
#include "InputCharStringDecodeStream.h"

class CharStringType1Interpreter
{
public:
	CharStringType1Interpreter(void);
	~CharStringType1Interpreter(void);

	EStatusCode Intepret(const Type1CharString& inCharStringToIntepret, IType1InterpreterImplementation* inImplementationHelper);

private:

	LongList mOperandStack;
	IType1InterpreterImplementation* mImplementationHelper;
	bool mGotEndChar;
	LongList mPostScriptOperandStack;

	EStatusCode ProcessCharString(InputCharStringDecodeStream* inCharStringToIntepret);
	bool IsOperator(Byte inBuffer);
	EStatusCode InterpretOperator(Byte inBuffer,InputCharStringDecodeStream* inCharStringToIntepret,bool& outGotEndExecutionCommand);
	EStatusCode InterpretNumber(Byte inBuffer,InputCharStringDecodeStream* inCharStringToIntepret);
	void ClearStack();
	EStatusCode DefaultCallOtherSubr();
	
	EStatusCode InterpretHStem();
	EStatusCode InterpretVStem();
	EStatusCode InterpretVMoveto();
	EStatusCode InterpretRLineto();
	EStatusCode InterpretHLineto();
	EStatusCode InterpretVLineto();
	EStatusCode InterpretRRCurveto();
	EStatusCode InterpretClosePath();
	EStatusCode InterpretCallSubr();
	EStatusCode InterpretReturn();
	EStatusCode InterpretHsbw();
	EStatusCode InterpretEndChar();
	EStatusCode InterpretRMoveto();
	EStatusCode InterpretHMoveto();
	EStatusCode InterpretVHCurveto();
	EStatusCode InterpretHVCurveto();
	EStatusCode InterpretDotSection();
	EStatusCode InterpretVStem3();
	EStatusCode InterpretHStem3();
	EStatusCode InterpretSeac();
	EStatusCode InterpretSbw();
	EStatusCode InterpretDiv();
	EStatusCode InterpretCallOtherSubr();
	EStatusCode InterpretPop();
	EStatusCode InterpretSetCurrentPoint();

};


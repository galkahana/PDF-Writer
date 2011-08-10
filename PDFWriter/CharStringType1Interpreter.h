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
#include "EPDFStatusCode.h"
#include "InputCharStringDecodeStream.h"

class CharStringType1Interpreter
{
public:
	CharStringType1Interpreter(void);
	~CharStringType1Interpreter(void);

	EPDFStatusCode Intepret(const Type1CharString& inCharStringToIntepret, IType1InterpreterImplementation* inImplementationHelper);

private:

	LongList mOperandStack;
	IType1InterpreterImplementation* mImplementationHelper;
	bool mGotEndChar;
	LongList mPostScriptOperandStack;

	EPDFStatusCode ProcessCharString(InputCharStringDecodeStream* inCharStringToIntepret);
	bool IsOperator(Byte inBuffer);
	EPDFStatusCode InterpretOperator(Byte inBuffer,InputCharStringDecodeStream* inCharStringToIntepret,bool& outGotEndExecutionCommand);
	EPDFStatusCode InterpretNumber(Byte inBuffer,InputCharStringDecodeStream* inCharStringToIntepret);
	void ClearStack();
	EPDFStatusCode DefaultCallOtherSubr();
	
	EPDFStatusCode InterpretHStem();
	EPDFStatusCode InterpretVStem();
	EPDFStatusCode InterpretVMoveto();
	EPDFStatusCode InterpretRLineto();
	EPDFStatusCode InterpretHLineto();
	EPDFStatusCode InterpretVLineto();
	EPDFStatusCode InterpretRRCurveto();
	EPDFStatusCode InterpretClosePath();
	EPDFStatusCode InterpretCallSubr();
	EPDFStatusCode InterpretReturn();
	EPDFStatusCode InterpretHsbw();
	EPDFStatusCode InterpretEndChar();
	EPDFStatusCode InterpretRMoveto();
	EPDFStatusCode InterpretHMoveto();
	EPDFStatusCode InterpretVHCurveto();
	EPDFStatusCode InterpretHVCurveto();
	EPDFStatusCode InterpretDotSection();
	EPDFStatusCode InterpretVStem3();
	EPDFStatusCode InterpretHStem3();
	EPDFStatusCode InterpretSeac();
	EPDFStatusCode InterpretSbw();
	EPDFStatusCode InterpretDiv();
	EPDFStatusCode InterpretCallOtherSubr();
	EPDFStatusCode InterpretPop();
	EPDFStatusCode InterpretSetCurrentPoint();

};


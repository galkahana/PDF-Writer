/*
   Source File : IPDFPageContentHandler.h


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

/*
	@TBD

*/

#include "EStatusCode.h"
#include "ContentBasicTypes.h"


class IPDFPageContentHandler
{
public:
	virtual ~IPDFPageContentHandler(){}
	
	// PDF Operators. For explanations on the meanings of each operator read Appendix A "Operator Summary" 
	// of the PDF Reference Manual (1.7)
	
	// path stroke/fill
	virtual PDFHummus::EStatusCode b() = 0;
	virtual PDFHummus::EStatusCode B() = 0;
	virtual PDFHummus::EStatusCode bStar() = 0;
	virtual PDFHummus::EStatusCode BStar() = 0;
	virtual PDFHummus::EStatusCode s() = 0;
	virtual PDFHummus::EStatusCode S() = 0;
	virtual PDFHummus::EStatusCode f() = 0;
	virtual PDFHummus::EStatusCode F() = 0;
	virtual PDFHummus::EStatusCode fStar() = 0;
	virtual PDFHummus::EStatusCode n() = 0;

	// path construction
	virtual PDFHummus::EStatusCode m(double inX,double inY) = 0;
	virtual PDFHummus::EStatusCode l(double inX,double inY) = 0;
	virtual PDFHummus::EStatusCode c(double inX1,double inY1,
									 double inX2, double inY2, 
									 double inX3, double inY3) = 0;
	virtual PDFHummus::EStatusCode v(double inX2,double inY2, 
									 double inX3, double inY3) = 0;
	virtual PDFHummus::EStatusCode y(double inX1,double inY1, 
									 double inX3, double inY3) = 0;
	virtual PDFHummus::EStatusCode h() = 0;
	virtual PDFHummus::EStatusCode re(double inLeft,double inBottom, 
									  double inWidth,double inHeight) = 0;

	// graphic state
	virtual PDFHummus::EStatusCode q() = 0;
	virtual PDFHummus::EStatusCode Q() = 0;
	virtual PDFHummus::EStatusCode cm(double inA, double inB, 
									  double inC, double inD, 
									  double inE, double inF) = 0;
	virtual PDFHummus::EStatusCode w(double inLineWidth) = 0;
	virtual PDFHummus::EStatusCode J(int inLineCapStyle) = 0;
	virtual PDFHummus::EStatusCode j(int inLineJoinStyle) = 0;
	virtual PDFHummus::EStatusCode M(double inMiterLimit) = 0;
	virtual PDFHummus::EStatusCode d(double* inDashArray, int inDashArrayLength, double inDashPhase) = 0;
	virtual PDFHummus::EStatusCode ri(const std::string& inRenderingIntentName) = 0;
	virtual PDFHummus::EStatusCode i(int inFlatness) = 0;
	virtual PDFHummus::EStatusCode gs(const std::string& inGraphicStateName) = 0;

	// color operators
	virtual PDFHummus::EStatusCode CS(const std::string& inColorSpaceName) = 0;
	virtual PDFHummus::EStatusCode cs(const std::string& inColorSpaceName) = 0;
	virtual PDFHummus::EStatusCode SC(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength,
									   const std::string& inPatternName) = 0;
	virtual PDFHummus::EStatusCode sc(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength,
									   const std::string& inPatternName) = 0;
	virtual PDFHummus::EStatusCode G(double inGray) = 0;
	virtual PDFHummus::EStatusCode g(double inGray) = 0;
	virtual PDFHummus::EStatusCode RG(double inR,double inG,double inB) = 0;
	virtual PDFHummus::EStatusCode rg(double inR,double inG,double inB) = 0;
	virtual PDFHummus::EStatusCode K(double inC,double inM,double inY,double inK) = 0;
	virtual PDFHummus::EStatusCode k(double inC,double inM,double inY,double inK) = 0;

	// clip operators
	virtual PDFHummus::EStatusCode W() = 0;
	virtual PDFHummus::EStatusCode WStar() = 0;

	// XObject usage
	virtual PDFHummus::EStatusCode Do(const std::string& inXObjectName) = 0;

	// Text state operators
	virtual PDFHummus::EStatusCode Tc(double inCharacterSpace) = 0;
	virtual PDFHummus::EStatusCode Tw(double inWordSpace) = 0;
	virtual PDFHummus::EStatusCode Tz(int inHorizontalScaling) = 0;
	virtual PDFHummus::EStatusCode TL(double inTextLeading) = 0;
	virtual PDFHummus::EStatusCode Tr(int inRenderingMode) = 0;
	virtual PDFHummus::EStatusCode Ts(double inFontRise) = 0;

	// Text object operators
	virtual PDFHummus::EStatusCode BT() = 0;
	virtual PDFHummus::EStatusCode ET() = 0;

	// Text positioning operators
	virtual PDFHummus::EStatusCode Td(double inTx, double inTy) = 0;
	virtual PDFHummus::EStatusCode TD(double inTx, double inTy) = 0;
	virtual PDFHummus::EStatusCode Tm(double inA, double inB, 
									  double inC, double inD, 
									  double inE, double inF) = 0;
	virtual PDFHummus::EStatusCode TStar() = 0;

	//
	// Text showing operators overriding library behavior
	//

	virtual PDFHummus::EStatusCode TfLow(const std::string& inFontName,double inFontSize) = 0; 

	virtual PDFHummus::EStatusCode TjLow(const std::string& inText) = 0;
	virtual PDFHummus::EStatusCode TjHexLow(const std::string& inText) = 0; 

	virtual PDFHummus::EStatusCode QuoteLow(const std::string& inText) = 0;
	virtual PDFHummus::EStatusCode QuoteHexLow(const std::string& inText) = 0;

	virtual PDFHummus::EStatusCode DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, 
												  const std::string& inText) = 0;
	virtual PDFHummus::EStatusCode DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, 
													 const std::string& inText) = 0; 

	virtual PDFHummus::EStatusCode TJLow(const StringOrDoubleList& inStringsAndSpacing) = 0;
	virtual PDFHummus::EStatusCode TJHexLow(const StringOrDoubleList& inStringsAndSpacing) = 0;
};

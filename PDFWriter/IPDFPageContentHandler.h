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
	virtual void b() = 0;
	virtual void B() = 0;
	virtual void bStar() = 0;
	virtual void BStar() = 0;
	virtual void s() = 0;
	virtual void S() = 0;
	virtual void f() = 0;
	virtual void F() = 0;
	virtual void fStar() = 0;
	virtual void n() = 0;

	// path construction
	virtual void m(double inX,double inY) = 0;
	virtual void l(double inX,double inY) = 0;
	virtual void c(	double inX1,double inY1,
					double inX2, double inY2, 
					double inX3, double inY3) = 0;
	virtual void v(	double inX2,double inY2, 
					double inX3, double inY3) = 0;
	virtual void y(	double inX1,double inY1, 
					double inX3, double inY3) = 0;
	virtual void h() = 0;
	virtual void re(double inLeft,double inBottom, double inWidth,double inHeight) = 0;

	// graphic state
	virtual void q() = 0;
	virtual PDFHummus::EStatusCode Q() = 0;
	virtual void cm(double inA, double inB, double inC, double inD, double inE, double inF) = 0;
	virtual void w(double inLineWidth) = 0;
	virtual void J(int inLineCapStyle) = 0;
	virtual void j(int inLineJoinStyle) = 0;
	virtual void M(double inMiterLimit) = 0;
	virtual void d(double* inDashArray, int inDashArrayLength, double inDashPhase) = 0;
	virtual void ri(const std::string& inRenderingIntentName) = 0;
	virtual void i(int inFlatness) = 0;
	virtual void gs(const std::string& inGraphicStateName) = 0;

	// color operators
	virtual void CS(const std::string& inColorSpaceName) = 0;
	virtual void cs(const std::string& inColorSpaceName) = 0;
	virtual void SC(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual void SCN(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual void SCN(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) = 0;
	virtual void sc(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual void scn(double* inColorComponents, int inColorComponentsLength) = 0;
	virtual void scn(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) = 0;
	virtual void G(double inGray) = 0;
	virtual void g(double inGray) = 0;
	virtual void RG(double inR,double inG,double inB) = 0;
	virtual void rg(double inR,double inG,double inB) = 0;
	virtual void K(double inC,double inM,double inY,double inK) = 0;
	virtual void k(double inC,double inM,double inY,double inK) = 0;

	// clip operators
	virtual void W() = 0;
	virtual void WStar() = 0;

	// XObject usage
	virtual void Do(const std::string& inXObjectName) = 0;

	// Text state operators
	virtual void Tc(double inCharacterSpace) = 0;
	virtual void Tw(double inWordSpace) = 0;
	virtual void Tz(int inHorizontalScaling) = 0;
	virtual void TL(double inTextLeading) = 0;
	virtual void Tr(int inRenderingMode) = 0;
	virtual void Ts(double inFontRise) = 0;

	// Text object operators
	virtual void BT() = 0;
	virtual void ET() = 0;

	// Text positioning operators
	virtual void Td(double inTx, double inTy) = 0;
	virtual void TD(double inTx, double inTy) = 0;
	virtual void Tm(double inA, double inB, double inC, double inD, double inE, double inF) = 0;
	virtual void TStar() = 0;

	//
	// Text showing operators overriding library behavior
	//

	virtual void TfLow(const std::string& inFontName,double inFontSize) = 0; 

	virtual void TjLow(const std::string& inText) = 0;
	virtual void TjHexLow(const std::string& inText) = 0; 

	virtual void QuoteLow(const std::string& inText) = 0;
	virtual void QuoteHexLow(const std::string& inText) = 0;

	virtual void DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) = 0;
	virtual void DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) = 0; 

	virtual void TJLow(const StringOrDoubleList& inStringsAndSpacing) = 0;
	virtual void TJHexLow(const StringOrDoubleList& inStringsAndSpacing) = 0;
};

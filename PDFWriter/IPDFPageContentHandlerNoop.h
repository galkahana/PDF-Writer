/*
   Source File : IPDFPageContentHandlerNoop.h


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

#include "IPDFPageContentHandler.h"

class IPDFPageContentHandlerNoop
	: public IPDFPageContentHandler
{
public:
	// path stroke/fill
	void b() {}
	void B() {}
	void bStar() {}
	void BStar() {}
	void s() {}
	void S() {}
	void f() {}
	void F() {}
	void fStar() {}
	void n() {}

	// path construction
	void m(double inX,double inY) {}
	void l(double inX,double inY) {}
	void c(	double inX1,double inY1, double inX2, double inY2, double inX3, double inY3) {}
	void v(	double inX2,double inY2, double inX3, double inY3){}
	void y(	double inX1,double inY1, double inX3, double inY3) {}
	void h() {}
	void re(double inLeft,double inBottom, double inWidth,double inHeight) {}

	// graphic state
	void q() {}
	PDFHummus::EStatusCode Q() {return PDFHummus::eSuccess;}
	void cm(double inA, double inB, double inC, double inD, double inE, double inF) {}
	void w(double inLineWidth) {}
	void J(int inLineCapStyle) {}
	void j(int inLineJoinStyle) {}
	void M(double inMiterLimit) {}
	void d(double* inDashArray, int inDashArrayLength, double inDashPhase) {}
	void ri(const std::string& inRenderingIntentName) {}
	void i(int inFlatness) {}
	void gs(const std::string& inGraphicStateName) {}

	// color operators
	void CS(const std::string& inColorSpaceName) {}
	void cs(const std::string& inColorSpaceName) {}
	void SC(double* inColorComponents, int inColorComponentsLength) {}
	void SCN(double* inColorComponents, int inColorComponentsLength) {}
	void SCN(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) {}
	void sc(double* inColorComponents, int inColorComponentsLength) {}
	void scn(double* inColorComponents, int inColorComponentsLength) {}
	void scn(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) {}
	void G(double inGray) {}
	void g(double inGray) {}
	void RG(double inR,double inG,double inB) {}
	void rg(double inR,double inG,double inB) {}
	void K(double inC,double inM,double inY,double inK) {}
	void k(double inC,double inM,double inY,double inK) {}

	// clip operators
	void W() {}
	void WStar() {}

	// XObject usage
	void Do(const std::string& inXObjectName) {}

	// Text state operators
	void Tc(double inCharacterSpace) {}
	void Tw(double inWordSpace) {}
	void Tz(int inHorizontalScaling) {}
	void TL(double inTextLeading) {}
	void Tr(int inRenderingMode) {}
	void Ts(double inFontRise) {}

	// Text object operators
	void BT() {}
	void ET() {}

	// Text positioning operators
	void Td(double inTx, double inTy) {}
	void TD(double inTx, double inTy) {}
	void Tm(double inA, double inB, double inC, double inD, double inE, double inF) {}
	void TStar() {}

	//
	// Text showing operators overriding library behavior
	//

	void TfLow(const std::string& inFontName,double inFontSize) {}

	void TjLow(const std::string& inText) {}
	void TjHexLow(const std::string& inText) {}

	void QuoteLow(const std::string& inText) {}
	void QuoteHexLow(const std::string& inText) {}

	void DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) {}
	void DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) {}

	void TJLow(const StringOrDoubleList& inStringsAndSpacing) {}
	void TJHexLow(const StringOrDoubleList& inStringsAndSpacing) {}
};


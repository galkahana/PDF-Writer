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


template <class Next>
class IContentContextExtenderAdapterPass
	: public IContentContextExtender
{
public:
	virtual Next& next() = 0;

	// path stroke/fill
	void b() { next().b(); }
	void B() { next().B(); }
	void bStar() { next().bStar(); }
	void BStar() { next().BStar(); }
	void s() { next().s(); }
	void S() { next().S(); }
	void f() { next().f(); }
	void F() { next().F(); }
	void fStar() { next().fStar(); }
	void n() { next().n(); }

	// path construction
	void m(double inX,double inY) { next().m(inX,inY); }
	void l(double inX,double inY) { next().l(inX,inY); }
	void c(	double inX1,double inY1, double inX2, double inY2, double inX3, double inY3) { next().c( inX1, inY1, inX2, inY2, inX3, inY3); }
	void v(	double inX2,double inY2, double inX3, double inY3) { next().v( inX2, inY2, inX3, inY3); }
	void y(	double inX1,double inY1, double inX3, double inY3) { next().y( inX1, inY1, inX3, inY3); }
	void h() { next().h(); }
	void re(double inLeft,double inBottom, double inWidth,double inHeight) { next().re( inLeft, inBottom, inWidth, inHeight); }

	// graphic state
	void q() { next().q(); }
	PDFHummus::EStatusCode Q() { return next().Q(); }
	void cm(double inA, double inB, double inC, double inD, double inE, double inF) { next().cm( inA, inB, inC, inD, inE, inF ); }
	void w(double inLineWidth) { next().w(inLineWidth); }
	void J(int inLineCapStyle) { next().J(inLineCapStyle); }
	void j(int inLineJoinStyle) { next().j(inLineJoinStyle); }
	void M(double inMiterLimit) { next().M(inMiterLimit); }
	void d(double* inDashArray, int inDashArrayLength, double inDashPhase) { next().d( inDashArray, inDashArrayLength, inDashPhase); }
	void ri(const std::string& inRenderingIntentName) { next().ri(inRenderingIntentName); }
	void i(int inFlatness) { next().i(inFlatness); }
	void gs(const std::string& inGraphicStateName) { next().gs(inGraphicStateName); }

	// color operators
	void CS(const std::string& inColorSpaceName) { next().CS(inColorSpaceName); }
	void cs(const std::string& inColorSpaceName) { next().cs(inColorSpaceName); }
	void SC(double* inColorComponents, int inColorComponentsLength) { next().SC(inColorComponents, inColorComponentsLength); }
	void SCN(double* inColorComponents, int inColorComponentsLength) { next().SCN(inColorComponents, inColorComponentsLength); }
	void SCN(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) { next().SCN(inColorComponents,inColorComponentsLength,inPatternName); }
	void sc(double* inColorComponents, int inColorComponentsLength) { next().sc(inColorComponents, inColorComponentsLength); }
	void scn(double* inColorComponents, int inColorComponentsLength) { next().scn(inColorComponents, inColorComponentsLength); }
	void scn(double* inColorComponents, int inColorComponentsLength,const std::string& inPatternName) { next().scn(inColorComponents,inColorComponentsLength,inPatternName); }
	void G(double inGray) { next().G(inGray); }
	void g(double inGray) { next().g(inGray); }
	void RG(double inR,double inG,double inB) { next().RG(inR,inG,inB); }
	void rg(double inR,double inG,double inB) { next().rg(inR,inG,inB); }
	void K(double inC,double inM,double inY,double inK) { next().K(inC,inM,inY,inK); }
	void k(double inC,double inM,double inY,double inK) { next().k(inC,inM,inY,inK); }

	// clip operators
	void W() { next().W(); }
	void WStar() { next().WStar(); }

	// XObject usage
	void Do(const std::string& inXObjectName) { next().Do(inXObjectName); }

	// Text state operators
	void Tc(double inCharacterSpace) { next().Tc(inCharacterSpace); }
	void Tw(double inWordSpace) { next().Tw(inWordSpace); }
	void Tz(int inHorizontalScaling) { next().Tz(inHorizontalScaling); }
	void TL(double inTextLeading) { next().TL(inTextLeading); }
	void Tr(int inRenderingMode) { next().Tr(inRenderingMode); }
	void Ts(double inFontRise) { next().Ts(inFontRise); }

	// Text object operators
	void BT() { next().BT(); }
	void ET() { next().ET(); }

	// Text positioning operators
	void Td(double inTx, double inTy) { next().Td(inTx, inTy); }
	void TD(double inTx, double inTy) { next().TD(inTx, inTy); }
	void Tm(double inA, double inB, double inC, double inD, double inE, double inF) { next().Tm(inA, inB, inC, inD, inE, inF); }
	void TStar() { next().TStar(); }

	//
	// Text showing operators overriding library behavior
	//

	void TfLow(const std::string& inFontName,double inFontSize) { next().TfLow(inFontName,inFontSize); }

	void TjLow(const std::string& inText) { next().TjLow(inText); }
	void TjHexLow(const std::string& inText) { next().TjHexLow(inText); }

	void QuoteLow(const std::string& inText) { next().QuoteLow(inText); }
	void QuoteHexLow(const std::string& inText) { next().QuoteHexLow(inText); }

	void DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) { next().DoubleQuoteLow(inWordSpacing, inCharacterSpacing, inText); }
	void DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, const std::string& inText) { next().DoubleQuoteHexLow(inWordSpacing, inCharacterSpacing, inText); }

	void TJLow(const StringOrDoubleList& inStringsAndSpacing) { next().TJLow(inStringsAndSpacing); }
	void TJHexLow(const StringOrDoubleList& inStringsAndSpacing) { next().TJHexLow(inStringsAndSpacing); }
};

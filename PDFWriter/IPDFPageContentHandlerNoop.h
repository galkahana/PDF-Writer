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
	PDFHummus::EStatusCode mStatus;
public:
	IPDFPageContentHandlerNoop( PDFHummus::EStatusCode inStatus = PDFHummus::eSuccess )
		: mStatus( inStatus )
	{
	}
public:
	// path stroke/fill
	PDFHummus::EStatusCode b() {return mStatus;}
	PDFHummus::EStatusCode B() {return mStatus;}
	PDFHummus::EStatusCode bStar() {return mStatus;}
	PDFHummus::EStatusCode BStar() {return mStatus;}
	PDFHummus::EStatusCode s() {return mStatus;}
	PDFHummus::EStatusCode S() {return mStatus;}
	PDFHummus::EStatusCode f() {return mStatus;}
	PDFHummus::EStatusCode F() {return mStatus;}
	PDFHummus::EStatusCode fStar() {return mStatus;}
	PDFHummus::EStatusCode n() {return mStatus;}

	// path construction
	PDFHummus::EStatusCode m(double inX,double inY) {return mStatus;}
	PDFHummus::EStatusCode l(double inX,double inY) {return mStatus;}
	PDFHummus::EStatusCode c(double inX1,double inY1, 
							 double inX2, double inY2, 
							 double inX3, double inY3) {return mStatus;}
	PDFHummus::EStatusCode v(double inX2,double inY2, 
							 double inX3, double inY3){return mStatus;}
	PDFHummus::EStatusCode y(double inX1,double inY1, 
							 double inX3, double inY3) {return mStatus;}
	PDFHummus::EStatusCode h() {return mStatus;}
	PDFHummus::EStatusCode re(double inLeft,double inBottom, 
							  double inWidth,double inHeight) {return mStatus;}

	// graphic state
	PDFHummus::EStatusCode q() {return mStatus;}
	PDFHummus::EStatusCode Q() {return mStatus;}
	PDFHummus::EStatusCode cm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) {return mStatus;}
	PDFHummus::EStatusCode w(double inLineWidth) {return mStatus;}
	PDFHummus::EStatusCode J(int inLineCapStyle) {return mStatus;}
	PDFHummus::EStatusCode j(int inLineJoinStyle) {return mStatus;}
	PDFHummus::EStatusCode M(double inMiterLimit) {return mStatus;}
	PDFHummus::EStatusCode d(double* inDashArray, int inDashArrayLength, 
							 double inDashPhase) {return mStatus;}
	PDFHummus::EStatusCode ri(const std::string& inRenderingIntentName) {return mStatus;}
	PDFHummus::EStatusCode i(int inFlatness) {return mStatus;}
	PDFHummus::EStatusCode gs(const std::string& inGraphicStateName) {return mStatus;}

	// color operators
	PDFHummus::EStatusCode CS(const std::string& inColorSpaceName) {return mStatus;}
	PDFHummus::EStatusCode cs(const std::string& inColorSpaceName) {return mStatus;}
	PDFHummus::EStatusCode SC(double* inColorComponents, int inColorComponentsLength) {return mStatus;}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength) {return mStatus;}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) {return mStatus;}
	PDFHummus::EStatusCode sc(double* inColorComponents, int inColorComponentsLength) {return mStatus;}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength) {return mStatus;}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) {return mStatus;}
	PDFHummus::EStatusCode G(double inGray) {return mStatus;}
	PDFHummus::EStatusCode g(double inGray) {return mStatus;}
	PDFHummus::EStatusCode RG(double inR,double inG,double inB) {return mStatus;}
	PDFHummus::EStatusCode rg(double inR,double inG,double inB) {return mStatus;}
	PDFHummus::EStatusCode K(double inC,double inM,double inY,double inK) {return mStatus;}
	PDFHummus::EStatusCode k(double inC,double inM,double inY,double inK) {return mStatus;}

	// clip operators
	PDFHummus::EStatusCode W() {return mStatus;}
	PDFHummus::EStatusCode WStar() {return mStatus;}

	// XObject usage
	PDFHummus::EStatusCode Do(const std::string& inXObjectName) {return mStatus;}

	// Text state operators
	PDFHummus::EStatusCode Tc(double inCharacterSpace) {return mStatus;}
	PDFHummus::EStatusCode Tw(double inWordSpace) {return mStatus;}
	PDFHummus::EStatusCode Tz(int inHorizontalScaling) {return mStatus;}
	PDFHummus::EStatusCode TL(double inTextLeading) {return mStatus;}
	PDFHummus::EStatusCode Tr(int inRenderingMode) {return mStatus;}
	PDFHummus::EStatusCode Ts(double inFontRise) {return mStatus;}

	// Text object operators
	PDFHummus::EStatusCode BT() {return mStatus;}
	PDFHummus::EStatusCode ET() {return mStatus;}

	// Text positioning operators
	PDFHummus::EStatusCode Td(double inTx, double inTy) {return mStatus;}
	PDFHummus::EStatusCode TD(double inTx, double inTy) {return mStatus;}
	PDFHummus::EStatusCode Tm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) {return mStatus;}
	PDFHummus::EStatusCode TStar() {return mStatus;}

	//
	// Text showing operators overriding library behavior
	//

	PDFHummus::EStatusCode TfLow(const std::string& inFontName,double inFontSize) {return mStatus;}

	PDFHummus::EStatusCode TjLow(const std::string& inText) {return mStatus;}
	PDFHummus::EStatusCode TjHexLow(const std::string& inText) {return mStatus;}

	PDFHummus::EStatusCode QuoteLow(const std::string& inText) {return mStatus;}
	PDFHummus::EStatusCode QuoteHexLow(const std::string& inText) {return mStatus;}

	PDFHummus::EStatusCode DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, 
										  const std::string& inText) {return mStatus;}
	PDFHummus::EStatusCode DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, 
											 const std::string& inText) {return mStatus;}

	PDFHummus::EStatusCode TJLow(const StringOrDoubleList& inStringsAndSpacing) {return mStatus;}
	PDFHummus::EStatusCode TJHexLow(const StringOrDoubleList& inStringsAndSpacing) {return mStatus;}
};


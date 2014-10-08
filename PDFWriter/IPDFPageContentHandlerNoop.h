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
	PDFHummus::EStatusCode b() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode B() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode bStar() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode BStar() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode s() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode S() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode f() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode F() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode fStar() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode n() {return PDFHummus::eSuccess;}

	// path construction
	PDFHummus::EStatusCode m(double inX,double inY) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode l(double inX,double inY) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode c(double inX1,double inY1, 
							 double inX2, double inY2, 
							 double inX3, double inY3) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode v(double inX2,double inY2, 
							 double inX3, double inY3){return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode y(double inX1,double inY1, 
							 double inX3, double inY3) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode h() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode re(double inLeft,double inBottom, 
							  double inWidth,double inHeight) {return PDFHummus::eSuccess;}

	// graphic state
	PDFHummus::EStatusCode q() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Q() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode cm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode w(double inLineWidth) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode J(int inLineCapStyle) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode j(int inLineJoinStyle) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode M(double inMiterLimit) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode d(double* inDashArray, int inDashArrayLength, 
							 double inDashPhase) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode ri(const std::string& inRenderingIntentName) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode i(int inFlatness) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode gs(const std::string& inGraphicStateName) {return PDFHummus::eSuccess;}

	// color operators
	PDFHummus::EStatusCode CS(const std::string& inColorSpaceName) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode cs(const std::string& inColorSpaceName) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode SC(double* inColorComponents, int inColorComponentsLength) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode sc(double* inColorComponents, int inColorComponentsLength) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode G(double inGray) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode g(double inGray) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode RG(double inR,double inG,double inB) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode rg(double inR,double inG,double inB) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode K(double inC,double inM,double inY,double inK) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode k(double inC,double inM,double inY,double inK) {return PDFHummus::eSuccess;}

	// clip operators
	PDFHummus::EStatusCode W() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode WStar() {return PDFHummus::eSuccess;}

	// XObject usage
	PDFHummus::EStatusCode Do(const std::string& inXObjectName) {return PDFHummus::eSuccess;}

	// Text state operators
	PDFHummus::EStatusCode Tc(double inCharacterSpace) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Tw(double inWordSpace) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Tz(int inHorizontalScaling) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode TL(double inTextLeading) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Tr(int inRenderingMode) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Ts(double inFontRise) {return PDFHummus::eSuccess;}

	// Text object operators
	PDFHummus::EStatusCode BT() {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode ET() {return PDFHummus::eSuccess;}

	// Text positioning operators
	PDFHummus::EStatusCode Td(double inTx, double inTy) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode TD(double inTx, double inTy) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode Tm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode TStar() {return PDFHummus::eSuccess;}

	//
	// Text showing operators overriding library behavior
	//

	PDFHummus::EStatusCode TfLow(const std::string& inFontName,double inFontSize) {return PDFHummus::eSuccess;}

	PDFHummus::EStatusCode TjLow(const std::string& inText) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode TjHexLow(const std::string& inText) {return PDFHummus::eSuccess;}

	PDFHummus::EStatusCode QuoteLow(const std::string& inText) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode QuoteHexLow(const std::string& inText) {return PDFHummus::eSuccess;}

	PDFHummus::EStatusCode DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, 
										  const std::string& inText) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, 
											 const std::string& inText) {return PDFHummus::eSuccess;}

	PDFHummus::EStatusCode TJLow(const StringOrDoubleList& inStringsAndSpacing) {return PDFHummus::eSuccess;}
	PDFHummus::EStatusCode TJHexLow(const StringOrDoubleList& inStringsAndSpacing) {return PDFHummus::eSuccess;}
};


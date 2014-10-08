/*
   Source File : IPDFPageContentHandlerPass.h


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


template <class Next>
class IPDFPageContentHandlerPass
	: public IPDFPageContentHandler
{
public: // IPDFPageContentHandler interface
	// path stroke/fill
	PDFHummus::EStatusCode b() { return pass( &Next::b ); }
	PDFHummus::EStatusCode B() { return pass( &Next::B ); }
	PDFHummus::EStatusCode bStar() { return pass( &Next::bStar ); }
	PDFHummus::EStatusCode BStar() { return pass( &Next::BStar ); }
	PDFHummus::EStatusCode s() { return pass( &Next::s ); }
	PDFHummus::EStatusCode S() { return pass( &Next::S ); }
	PDFHummus::EStatusCode f() { return pass( &Next::f ); }
	PDFHummus::EStatusCode F() { return pass( &Next::F ); }
	PDFHummus::EStatusCode fStar() { return pass( &Next::fStar ); }
	PDFHummus::EStatusCode n() { return pass( &Next::n ); }

	// path construction
	PDFHummus::EStatusCode m(double inX,double inY) { return pass( &Next::m, inX, inY); }
	PDFHummus::EStatusCode l(double inX,double inY) { return pass( &Next::l, inX,inY); }
	PDFHummus::EStatusCode c(double inX1,double inY1, 
							 double inX2, double inY2, 
							 double inX3, double inY3) 
	{ 
		return pass( &Next::c, inX1, inY1, inX2, inY2, inX3, inY3); 
	}
	PDFHummus::EStatusCode v(double inX2,double inY2, 
							 double inX3, double inY3) { return pass( &Next::v, inX2, inY2, inX3, inY3); }
	PDFHummus::EStatusCode y(double inX1,double inY1, 
							 double inX3, double inY3) { return pass( &Next::y, inX1, inY1, inX3, inY3); }
	PDFHummus::EStatusCode h() { return pass( &Next::h ); }
	PDFHummus::EStatusCode re(double inLeft,double inBottom, 
							  double inWidth,double inHeight) 
	{ 
		return pass( &Next::re, inLeft, inBottom, inWidth, inHeight); 
	}
	

	// graphic state
	PDFHummus::EStatusCode q() { return pass( &Next::q ); }
	PDFHummus::EStatusCode Q() { return pass( &Next::Q ); }
	PDFHummus::EStatusCode cm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) 
	{ 
		return pass( &Next::cm, inA, inB, inC, inD, inE, inF ); 
	}
	PDFHummus::EStatusCode w(double inLineWidth) { return pass( &Next::w, inLineWidth); }
	PDFHummus::EStatusCode J(int inLineCapStyle) { return pass( &Next::J, inLineCapStyle); }
	PDFHummus::EStatusCode j(int inLineJoinStyle) { return pass( &Next::j, inLineJoinStyle); }
	PDFHummus::EStatusCode M(double inMiterLimit) { return pass( &Next::M, inMiterLimit); }
	PDFHummus::EStatusCode d(double* inDashArray, int inDashArrayLength, 
							 double inDashPhase) 
	{ 
		return pass( &Next::d, inDashArray, inDashArrayLength, inDashPhase); 
	}
	PDFHummus::EStatusCode ri(const std::string& inRenderingIntentName) 
	{ 
		return pass( &Next::ri, inRenderingIntentName); 
	}
	PDFHummus::EStatusCode i(int inFlatness) { return pass( &Next::i, inFlatness); }
	PDFHummus::EStatusCode gs(const std::string& inGraphicStateName) { return pass( &Next::gs, inGraphicStateName); }

	// color operators
	PDFHummus::EStatusCode CS(const std::string& inColorSpaceName) { return pass( &Next::CS, inColorSpaceName); }
	PDFHummus::EStatusCode cs(const std::string& inColorSpaceName) { return pass( &Next::cs, inColorSpaceName); }
	PDFHummus::EStatusCode SC(double* inColorComponents, int inColorComponentsLength) 
	{ 
		return pass( &Next::SC, inColorComponents, inColorComponentsLength); 
	}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength) 
	{ 
		return pass( &Next::SCN, inColorComponents, inColorComponentsLength); 
	}
	PDFHummus::EStatusCode SCN(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) 
	{ 
		return pass( &Next::SCN, inColorComponents,inColorComponentsLength,inPatternName); 
	}
	PDFHummus::EStatusCode sc(double* inColorComponents, int inColorComponentsLength) 
	{
		return pass( &Next::sc, inColorComponents, inColorComponentsLength); 
	}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength) 
	{ 
		return pass( &Next::scn, inColorComponents, inColorComponentsLength); 
	}
	PDFHummus::EStatusCode scn(double* inColorComponents, int inColorComponentsLength,
							   const std::string& inPatternName) 
	{ 
		return pass( &Next::scn, inColorComponents,inColorComponentsLength,inPatternName); 
	}
	PDFHummus::EStatusCode G(double inGray) { return pass( &Next::G, inGray); }
	PDFHummus::EStatusCode g(double inGray) { return pass( &Next::g, inGray); }
	PDFHummus::EStatusCode RG(double inR,double inG,double inB) { return pass( &Next::RG, inR,inG,inB); }
	PDFHummus::EStatusCode rg(double inR,double inG,double inB) { return pass( &Next::rg, inR,inG,inB); }
	PDFHummus::EStatusCode K(double inC,double inM,double inY,double inK) { return pass( &Next::K,inC,inM,inY,inK); }
	PDFHummus::EStatusCode k(double inC,double inM,double inY,double inK) { return pass( &Next::k,inC,inM,inY,inK); }

	// clip operators
	PDFHummus::EStatusCode W() { return pass( &Next::W ); }
	PDFHummus::EStatusCode WStar() { return pass( &Next::WStar ); }

	// XObject usage
	PDFHummus::EStatusCode Do(const std::string& inXObjectName) { return pass( &Next::Do,inXObjectName); }

	// Text state operators
	PDFHummus::EStatusCode Tc(double inCharacterSpace) { return pass( &Next::Tc,inCharacterSpace); }
	PDFHummus::EStatusCode Tw(double inWordSpace) { return pass( &Next::Tw,inWordSpace); }
	PDFHummus::EStatusCode Tz(int inHorizontalScaling) { return pass( &Next::Tz,inHorizontalScaling); }
	PDFHummus::EStatusCode TL(double inTextLeading) { return pass( &Next::TL,inTextLeading); }
	PDFHummus::EStatusCode Tr(int inRenderingMode) { return pass( &Next::Tr,inRenderingMode); }
	PDFHummus::EStatusCode Ts(double inFontRise) { return pass( &Next::Ts,inFontRise); }

	// Text object operators
	PDFHummus::EStatusCode BT() { return pass( &Next::BT ); }
	PDFHummus::EStatusCode ET() { return pass( &Next::ET ); }

	// Text positioning operators
	PDFHummus::EStatusCode Td(double inTx, double inTy) { return pass( &Next::Td,inTx, inTy); }
	PDFHummus::EStatusCode TD(double inTx, double inTy) { return pass( &Next::TD,inTx, inTy); }
	PDFHummus::EStatusCode Tm(double inA, double inB, 
							  double inC, double inD, 
							  double inE, double inF) { return pass( &Next::Tm,inA, inB, inC, inD, inE, inF); }
	PDFHummus::EStatusCode TStar() { return pass( &Next::TStar ); }

	//
	// Text showing operators overriding library behavior
	//

	PDFHummus::EStatusCode TfLow(const std::string& inFontName,double inFontSize) 
	{ 
		return pass( &Next::TfLow,inFontName,inFontSize); 
	}

	PDFHummus::EStatusCode TjLow(const std::string& inText) { return pass( &Next::TjLow,inText); }
	PDFHummus::EStatusCode TjHexLow(const std::string& inText) { return pass( &Next::TjHexLow,inText); }

	PDFHummus::EStatusCode QuoteLow(const std::string& inText) { return pass( &Next::QuoteLow,inText); }
	PDFHummus::EStatusCode QuoteHexLow(const std::string& inText) { return pass( &Next::QuoteHexLow,inText); }

	PDFHummus::EStatusCode DoubleQuoteLow(double inWordSpacing, double inCharacterSpacing, 
										  const std::string& inText) 
	{ 
		return pass( &Next::DoubleQuoteLow,inWordSpacing, inCharacterSpacing, inText); 
	}
	PDFHummus::EStatusCode DoubleQuoteHexLow(double inWordSpacing, double inCharacterSpacing, 
											 const std::string& inText) 
	{ 
		return pass( &Next::DoubleQuoteHexLow,inWordSpacing, inCharacterSpacing, inText); 
	}

	PDFHummus::EStatusCode TJLow(const StringOrDoubleList& inStringsAndSpacing) 
	{ 
		return pass( &Next::TJLow,inStringsAndSpacing); 
	}
	PDFHummus::EStatusCode TJHexLow(const StringOrDoubleList& inStringsAndSpacing) 
	{ 
		return pass( &Next::TJHexLow, inStringsAndSpacing); 
	}

private:
	virtual Next& next() = 0;

protected:
	// Here we create an infrastructure for delegating calls regardless their return type
	// We return methods' status code while for void methods we return PDFHummus::eSuccess.
	// Unfortunately without perfect forwarding we must list all the parameter combinations.

	// Methods returning PDFHummus::EStatusCode:

	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)() )
	{
		return (next().*inMethod)();
	}


	template<typename T1>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1 ), T1 arg1 )
	{
		return (next().*inMethod)( arg1 );
	}

	template<typename T1>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1 const& ), T1 const& arg1 )
	{
		return (next().*inMethod)( arg1 );
	}

	template<typename T1, typename T2>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1, T2 ), T1 arg1, T2 arg2 )
	{
		return (next().*inMethod)( arg1, arg2 );
	}

	template<typename T1, typename T2>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1 const&, T2 ), T1 const& arg1, T2 arg2 )
	{
		return (next().*inMethod)( arg1, arg2 );
	}

	template<typename T1, typename T2, typename T3>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1, T2, T3 ), 
								 T1 arg1, T2 arg2, T3 arg3 )
	{
		return (next().*inMethod)( arg1, arg2, arg3 );
	}

	template<typename T1, typename T2, typename T3>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1, T2, T3 const& ), 
								 T1 arg1, T2 arg2, T3 const& arg3 )
	{
		return (next().*inMethod)( arg1, arg2, arg3 );
	}

	template<typename T1, typename T2, typename T3, typename T4>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1, T2, T3, T4 ), 
								 T1 arg1, T2 arg2, T3 arg3, T4 arg4 )
	{
		return (next().*inMethod)( arg1, arg2, arg3, arg4 );
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	PDFHummus::EStatusCode pass( PDFHummus::EStatusCode (Next::*inMethod)( T1, T2, T3, T4, T5, T6 ), 
								 T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6 )
	{
		return (next().*inMethod)( arg1, arg2, arg3, arg4, arg5, arg6 );
	}



	// Methods with void return type always return PDFHummus::eSuccess

	PDFHummus::EStatusCode pass( void (Next::*inMethod)() )
	{
		(next().*inMethod)();
		return PDFHummus::eSuccess;
	}

	template<typename T1>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1 ), T1 arg1 )
	{
		(next().*inMethod)( arg1 );
		return PDFHummus::eSuccess;
	}

	template<typename T1>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1 const& ), T1 const& arg1 )
	{
		(next().*inMethod)( arg1 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1, T2 ), T1 arg1, T2 arg2 )
	{
		(next().*inMethod)( arg1, arg2 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1 const&, T2 ), T1 const& arg1, T2 arg2 )
	{
		(next().*inMethod)( arg1, arg2 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2, typename T3>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1, T2, T3 ), 
								 T1 arg1, T2 arg2, T3 arg3 )
	{
		(next().*inMethod)( arg1, arg2, arg3 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2, typename T3>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1, T2, T3 const& ), 
								 T1 arg1, T2 arg2, T3 const& arg3 )
	{
		(next().*inMethod)( arg1, arg2, arg3 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2, typename T3, typename T4>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1, T2, T3, T4 ), 
								 T1 arg1, T2 arg2, T3 arg3, T4 arg4 )
	{
		(next().*inMethod)( arg1, arg2, arg3, arg4 );
		return PDFHummus::eSuccess;
	}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	PDFHummus::EStatusCode pass( void (Next::*inMethod)( T1, T2, T3, T4, T5, T6 ), 
								 T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6 )
	{
		(next().*inMethod)( arg1, arg2, arg3, arg4, arg5, arg6 );
		return PDFHummus::eSuccess;
	}


};

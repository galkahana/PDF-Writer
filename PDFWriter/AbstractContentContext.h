#pragma once

/*
	abstract implementation for content context.
	writes functions, and function parameters.

	exculdes taking care of streams and the identity of the resource dictionary.

	use SetPDFStreamForWrite to setup the stream for write (may be using multiple times)
	Implement GetResourcesDictionary to return the relevant resources dictionary for the content
	Optionally Implement RenewStreamConnection, which is called before any operator is written, in order to make sure there's a stream there to write to

*/

#include "EStatusCode.h"
#include "PrimitiveObjectsWriter.h"
#include "GraphicStateStack.h"
#include "GlyphUnicodeMapping.h"
#include <string>
#include <list>

using namespace std;

class ObjectsContext;
class PDFStream;
class ResourcesDictionary;
class PDFImageXObject;
class ITextCommand;

template <typename T>
struct SomethingOrDouble
{
	T SomeValue;
	double DoubleValue;

	bool IsDouble; // true - double, false - other

	SomethingOrDouble(T inSome){SomeValue = inSome;IsDouble = false;}
	SomethingOrDouble(double inDouble){DoubleValue = inDouble;IsDouble = true;}
};

typedef SomethingOrDouble<string> StringOrDouble;
typedef SomethingOrDouble<wstring> WStringOrDouble;
typedef SomethingOrDouble<GlyphUnicodeMappingList> GlyphUnicodeMappingListOrDouble;

typedef list<StringOrDouble> StringOrDoubleList;
typedef list<WStringOrDouble> WStringOrDoubleList;
typedef list<GlyphUnicodeMappingListOrDouble> GlyphUnicodeMappingListOrDoubleList;

typedef list<wstring> WStringList;

class AbstractContentContext
{
public:
	AbstractContentContext(void);
	virtual ~AbstractContentContext(void);

	// PDF Operators. For explanations on the meanings of each operator read Appendix A "Operator Summary" of the PDF Reference Manual (1.7)
	
	// path stroke/fill
	void b();
	void B();
	void bStar();
	void BStar();
	void s();
	void S();
	void f();
	void F();
	void fStar();
	void n();

	// path construction
	void m(double inX,double inY);
	void l(double inX,double inY);
	void c(	double inX1,double inY1, 
			double inX2, double inY2, 
			double inX3, double inY3);
	void v(	double inX2,double inY2, 
			double inX3, double inY3);
	void y(	double inX1,double inY1, 
			double inX3, double inY3);
	void h();
	void re(double inLeft,double inBottom, double inWidth,double inHeight);

	// graphic state
	void q();
	EStatusCode Q(); // Status code returned, in case there's inbalance in "q-Q"s
	void cm(double inA, double inB, double inC, double inD, double inE, double inF);
	void w(double inLineWidth);
	void J(int inLineCapStyle);
	void j(int inLineJoinStyle);
	void M(double inMiterLimit);
	void d(int* inDashArray, int inDashArrayLength,int inDashPhase);
	void ri(const string& inRenderingIntentName);
	void i(int inFlatness);
	void gs(const string& inGraphicStateName);

	// color operators
	void CS(const string& inColorSpaceName);
	void cs(const string& inColorSpaceName);
	void SC(double* inColorComponents, int inColorComponentsLength);
	void SCN(double* inColorComponents, int inColorComponentsLength);
	void SCN(double* inColorComponents, int inColorComponentsLength,const string& inPatternName);
	void sc(double* inColorComponents, int inColorComponentsLength);
	void scn(double* inColorComponents, int inColorComponentsLength);
	void scn(double* inColorComponents, int inColorComponentsLength,const string& inPatternName);
	void G(double inGray);
	void g(double inGray);
	void RG(double inR,double inG,double inB);
	void rg(double inR,double inG,double inB);
	void K(double inC,double inM,double inY,double inK);
	void k(double inC,double inM,double inY,double inK);

	// clip operators
	void W();
	void WStar();

	// XObject usage
	void Do(const string& inXObjectName);

	// Text state operators
	void Tc(double inCharacterSpace);
	void Tw(double inWordSpace);
	void Tz(int inHorizontalScaling);
	void TL(double inTextLeading);
	void Tr(int inRenderingMode);
	void Ts(double inFontRise);

	// Text object operators
	void BT();
	void ET();

	// Text positioning operators
	void Td(double inTx, double inTy);
	void TD(double inTx, double inTy);
	void Tm(double inA, double inB, double inC, double inD, double inE, double inF);
	void TStar();

	//
	// Text showing operators using the library handling of fonts with unicode text using UTF 16
	//

	// Set the font for later text writing command to the font in reference.
	// to create such a font use PDF Writer GetFontForFile.
	// Note that placing an actual Tf command (and including in resources dictionary) will
	// only occur when actually placing text.
	void Tf(PDFUsedFont* inFontReference,double inFontSize);

	// place text to the current set font with Tf
	// will return error if no font was set, or that one of the glyphs
	// didn't succeed in encoding.
	// input parameter is UTF-16 encoded
	EStatusCode Tj(const wstring& inText);

	// The rest of the text operators, handled by the library handing of font. text is in UTF16
	EStatusCode Quote(const wstring& inText);
	EStatusCode DoubleQuote(double inWordSpacing, double inCharacterSpacing, const wstring& inText);
	EStatusCode TJ(const WStringOrDoubleList& inStringsAndSpacing); 

	//
	// Text showing operators using the library handling of fonts with direct glyph selection
	//
	
	// For Direct glyph selections (if you don't like my UTF16 encoding), use the following commands.
	// each command accepts a list of glyphs. each glyph is mapped to its matching unicode values. 
	// a glyph may have more than one unicode value in case it reperesents a series of Characters.

	EStatusCode Tj(const GlyphUnicodeMappingList& inText);
	EStatusCode Quote(const GlyphUnicodeMappingList& inText);
	EStatusCode DoubleQuote(double inWordSpacing, double inCharacterSpacing, const GlyphUnicodeMappingList& inText);
	EStatusCode TJ(const GlyphUnicodeMappingListOrDoubleList& inStringsAndSpacing); 

	//
	// Text showing operators overriding library behavior
	//

	// Low level text showing operators. use them only if you completly take chare of
	// font and text usage

	// Low level setting of font. for the high level version, see below
	void Tf(const string& inFontName,double inFontSize); 

	// first version of Tj writes the string in literal string paranthesis, 
	// second version of Tj writes the string in hex string angle brackets
	void Tj(const string& inText);
	void TjHex(const string& inText); 

	void Quote(const string& inText); // matches the operator '
	void QuoteHex(const string& inText);

	void DoubleQuote(double inWordSpacing, double inCharacterSpacing, const string& inText); // matches the operator "
	void DoubleQuoteHex(double inWordSpacing, double inCharacterSpacing, const string& inText); 

	// similar to the TJ PDF command, TJ() recieves an input an array of items which
	// can be either a string or a double
	void TJ(const StringOrDoubleList& inStringsAndSpacing);
	void TJHex(const StringOrDoubleList& inStringsAndSpacing);


protected:

	// Derived classes should use this method to update the stream for writing
	void SetPDFStreamForWrite(PDFStream* inStream);

private:
	// Derived classes should use this method to retrive the content resource dictionary, for updating procsets 'n such
	virtual ResourcesDictionary* GetResourcesDictionary() = 0;
	// Derived classes should optionally use this method if the stream needs updating (use calls to SetPDFStreamForWrite for this purpose)
	virtual void RenewStreamConnection() {};

	PrimitiveObjectsWriter mPrimitiveWriter;

	// graphic stack to monitor high-level graphic usage (now - fonts)
	GraphicStateStack mGraphicStack;

	void AssertProcsetAvailable(const string& inProcsetName);

	EStatusCode WriteTextCommandWithEncoding(const wstring& inUnicodeText,ITextCommand* inTextCommand);
	EStatusCode WriteTextCommandWithDirectGlyphSelection(const GlyphUnicodeMappingList& inText,ITextCommand* inTextCommand);
};

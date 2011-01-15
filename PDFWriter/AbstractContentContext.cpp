#include "AbstractContentContext.h"
#include "PDFStream.h"
#include "ResourcesDictionary.h"
#include "PDFImageXObject.h"
#include "ProcsetResourcesConstants.h"
#include "PDFUsedFont.h"
#include "Trace.h"
#include "OutputStringBufferStream.h"
#include "SafeBufferMacrosDefs.h"

AbstractContentContext::AbstractContentContext(void)
{
}

AbstractContentContext::~AbstractContentContext(void)
{
}

void AbstractContentContext::SetPDFStreamForWrite(PDFStream* inStream)
{
	mPrimitiveWriter.SetStreamForWriting(inStream->GetWriteStream());	
}

void AbstractContentContext::AssertProcsetAvailable(const string& inProcsetName)
{
	GetResourcesDictionary()->AddProcsetResource(inProcsetName);	
}


static const string scAddRectangleToPath = "re";
void AbstractContentContext::re(double inLeft,double inBottom, double inWidth,double inHeight)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inLeft);
	mPrimitiveWriter.WriteDouble(inBottom);
	mPrimitiveWriter.WriteDouble(inWidth);
	mPrimitiveWriter.WriteDouble(inHeight);
	mPrimitiveWriter.WriteKeyword(scAddRectangleToPath);
}

static const string scFill = "f";
void AbstractContentContext::f()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword(scFill);
}

void AbstractContentContext::S()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("S");
}

void AbstractContentContext::s()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("s");
}

void AbstractContentContext::F()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("F");
}

void AbstractContentContext::fStar()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("f*");
}

void AbstractContentContext::B()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("B");
}

void AbstractContentContext::BStar()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("B*");
}

void AbstractContentContext::b()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("b");
}

void AbstractContentContext::bStar()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("b*");
}

void AbstractContentContext::n()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("n");
}

void AbstractContentContext::m(double inX,double inY)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteKeyword("m");
}

void AbstractContentContext::l(double inX,double inY)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteKeyword("l");
}

void AbstractContentContext::c(	double inX1,double inY1, 
							double inX2, double inY2, 
							double inX3, double inY3)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX1);
	mPrimitiveWriter.WriteDouble(inY1);
	mPrimitiveWriter.WriteDouble(inX2);
	mPrimitiveWriter.WriteDouble(inY2);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("c");
}

void AbstractContentContext::v(	double inX2,double inY2, 
							double inX3, double inY3)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX2);
	mPrimitiveWriter.WriteDouble(inY2);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("v");
}

void AbstractContentContext::y(	double inX1,double inY1, 
							double inX3, double inY3)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX1);
	mPrimitiveWriter.WriteDouble(inY1);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("y");
}

void AbstractContentContext::h()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("h");
}

void AbstractContentContext::q()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("q");
	mGraphicStack.Push();
}

EStatusCode AbstractContentContext::Q()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("Q");
	return mGraphicStack.Pop();
}

void AbstractContentContext::cm(double inA, double inB, double inC, double inD, double inE, double inF)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inA);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inD);
	mPrimitiveWriter.WriteDouble(inE);
	mPrimitiveWriter.WriteDouble(inF);
	mPrimitiveWriter.WriteKeyword("cm");
}

void AbstractContentContext::w(double inLineWidth)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inLineWidth);
	mPrimitiveWriter.WriteKeyword("w");
}

void AbstractContentContext::J(int inLineCapStyle)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inLineCapStyle);
	mPrimitiveWriter.WriteKeyword("J");
}

void AbstractContentContext::j(int inLineJoinStyle)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inLineJoinStyle);
	mPrimitiveWriter.WriteKeyword("j");
}

void AbstractContentContext::M(double inMiterLimit)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inMiterLimit);
	mPrimitiveWriter.WriteKeyword("M");
}

void AbstractContentContext::d(int* inDashArray, int inDashArrayLength,int inDashPhase)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.StartArray();
	for(int i=0;i<inDashArrayLength;++i)
		mPrimitiveWriter.WriteInteger(inDashArray[i]);
	mPrimitiveWriter.EndArray(eTokenSeparatorSpace);
	mPrimitiveWriter.WriteInteger(inDashPhase);
	mPrimitiveWriter.WriteKeyword("d");
}

void AbstractContentContext::ri(const string& inRenderingIntentName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inRenderingIntentName);
	mPrimitiveWriter.WriteKeyword("ri");
}

void AbstractContentContext::i(int inFlatness)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inFlatness);
	mPrimitiveWriter.WriteKeyword("i");
}

void AbstractContentContext::gs(const string& inGraphicStateName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inGraphicStateName);
	mPrimitiveWriter.WriteKeyword("gs");
}


void AbstractContentContext::CS(const string& inColorSpaceName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inColorSpaceName);
	mPrimitiveWriter.WriteKeyword("CS");
}

void AbstractContentContext::cs(const string& inColorSpaceName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inColorSpaceName);
	mPrimitiveWriter.WriteKeyword("cs");
}

void AbstractContentContext::SC(double* inColorComponents, int inColorComponentsLength)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("SC");
}

void AbstractContentContext::SCN(double* inColorComponents, int inColorComponentsLength)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("SCN");
}

void AbstractContentContext::SCN(double* inColorComponents, int inColorComponentsLength,const string& inPatternName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteName(inPatternName);
	mPrimitiveWriter.WriteKeyword("SCN");
}

void AbstractContentContext::sc(double* inColorComponents, int inColorComponentsLength)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("sc");
}

void AbstractContentContext::scn(double* inColorComponents, int inColorComponentsLength)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("scn");
}

void AbstractContentContext::scn(double* inColorComponents, int inColorComponentsLength,const string& inPatternName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteName(inPatternName);
	mPrimitiveWriter.WriteKeyword("scn");
}

void AbstractContentContext::G(double inGray)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inGray);
	mPrimitiveWriter.WriteKeyword("G");
}

void AbstractContentContext::g(double inGray)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inGray);
	mPrimitiveWriter.WriteKeyword("g");
}

void AbstractContentContext::RG(double inR,double inG,double inB)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inR);
	mPrimitiveWriter.WriteDouble(inG);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteKeyword("RG");
}

void AbstractContentContext::rg(double inR,double inG,double inB)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inR);
	mPrimitiveWriter.WriteDouble(inG);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteKeyword("rg");
}

void AbstractContentContext::K(double inC,double inM,double inY,double inK)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inM);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteDouble(inK);
	mPrimitiveWriter.WriteKeyword("K");
}

void AbstractContentContext::k(double inC,double inM,double inY,double inK)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inM);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteDouble(inK);
	mPrimitiveWriter.WriteKeyword("k");
}

void AbstractContentContext::W()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("W");
}

void AbstractContentContext::WStar()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("W*");
}

void AbstractContentContext::Do(const string& inXObjectName)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inXObjectName);	
	mPrimitiveWriter.WriteKeyword("Do");	
}

void AbstractContentContext::Tc(double inCharacterSpace)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inCharacterSpace);
	mPrimitiveWriter.WriteKeyword("Tc");
}

void AbstractContentContext::Tw(double inWordSpace)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inWordSpace);
	mPrimitiveWriter.WriteKeyword("Tw");
}

void AbstractContentContext::Tz(int inHorizontalScaling)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteInteger(inHorizontalScaling);
	mPrimitiveWriter.WriteKeyword("Tz");
}

void AbstractContentContext::TL(double inTextLeading)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inTextLeading);
	mPrimitiveWriter.WriteKeyword("TL");
}

void AbstractContentContext::Tf(const string& inFontName,double inFontSize)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteName(inFontName);
	mPrimitiveWriter.WriteDouble(inFontSize);
	mPrimitiveWriter.WriteKeyword("Tf");

	mGraphicStack.GetCurrentState().mPlacedFontName = inFontName;
	mGraphicStack.GetCurrentState().mFontSize = inFontSize;
}

void AbstractContentContext::Tr(int inRenderingMode)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteInteger(inRenderingMode);
	mPrimitiveWriter.WriteKeyword("Tr");
}

void AbstractContentContext::Ts(double inFontRise)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inFontRise);
	mPrimitiveWriter.WriteKeyword("Ts");
}

void AbstractContentContext::BT()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteKeyword("BT");
}

void AbstractContentContext::ET()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteKeyword("ET");
}

void AbstractContentContext::Td(double inTx, double inTy)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inTx);
	mPrimitiveWriter.WriteDouble(inTy);
	mPrimitiveWriter.WriteKeyword("Td");
}

void AbstractContentContext::TD(double inTx, double inTy)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inTx);
	mPrimitiveWriter.WriteDouble(inTy);
	mPrimitiveWriter.WriteKeyword("TD");
}

void AbstractContentContext::Tm(double inA, double inB, double inC, double inD, double inE, double inF)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inA);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inD);
	mPrimitiveWriter.WriteDouble(inE);
	mPrimitiveWriter.WriteDouble(inF);
	mPrimitiveWriter.WriteKeyword("Tm");
}

void AbstractContentContext::TStar()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteKeyword("T*");
}


void AbstractContentContext::Tj(const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteLiteralString(inText);
	mPrimitiveWriter.WriteKeyword("Tj");
}

void AbstractContentContext::TjHex(const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteHexString(inText);
	mPrimitiveWriter.WriteKeyword("Tj");
}

void AbstractContentContext::Quote(const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteLiteralString(inText);
	mPrimitiveWriter.WriteKeyword("'");
}

void AbstractContentContext::QuoteHex(const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteHexString(inText);
	mPrimitiveWriter.WriteKeyword("Quote");
}

void AbstractContentContext::DoubleQuote(	double inWordSpacing, 
											double inCharacterSpacing, 
											const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inWordSpacing);
	mPrimitiveWriter.WriteDouble(inCharacterSpacing);
	mPrimitiveWriter.WriteLiteralString(inText);
	mPrimitiveWriter.WriteKeyword("\"");
}

void AbstractContentContext::DoubleQuoteHex(double inWordSpacing, double inCharacterSpacing, const string& inText)
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.WriteDouble(inWordSpacing);
	mPrimitiveWriter.WriteDouble(inCharacterSpacing);
	mPrimitiveWriter.WriteHexString(inText);
	mPrimitiveWriter.WriteKeyword("\"");
}

void AbstractContentContext::TJ(const StringOrDoubleList& inStringsAndSpacing)
{
	StringOrDoubleList::const_iterator it = inStringsAndSpacing.begin();
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.StartArray();

	for(; it != inStringsAndSpacing.end();++it)
	{
		if(it->IsDouble)
			mPrimitiveWriter.WriteDouble(it->DoubleValue);
		else
			mPrimitiveWriter.WriteLiteralString(it->SomeValue);
	}
	
	mPrimitiveWriter.EndArray(eTokenSeparatorSpace);

	mPrimitiveWriter.WriteKeyword("TJ");
}

void AbstractContentContext::TJHex(const StringOrDoubleList& inStringsAndSpacing)
{
	StringOrDoubleList::const_iterator it = inStringsAndSpacing.begin();
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);
	AssertProcsetAvailable(KProcsetText);

	mPrimitiveWriter.StartArray();

	for(; it != inStringsAndSpacing.end();++it)
	{
		if(it->IsDouble)
			mPrimitiveWriter.WriteDouble(it->DoubleValue);
		else
			mPrimitiveWriter.WriteHexString(it->SomeValue);
	}
	
	mPrimitiveWriter.EndArray(eTokenSeparatorSpace);

	mPrimitiveWriter.WriteKeyword("TJ");
}

void AbstractContentContext::Tf(PDFUsedFont* inFontReference,double inFontSize)
{
	mGraphicStack.GetCurrentState().mFont = inFontReference;
	mGraphicStack.GetCurrentState().mFontSize = inFontSize;
}

class ITextCommand
{
public:

	virtual void WriteHexStringCommand(const string& inStringToWrite) = 0;
	virtual void WriteLiteralStringCommand(const string& inStringToWrite) = 0;
};

EStatusCode AbstractContentContext::WriteTextCommandWithEncoding(const wstring& inUnicodeText,ITextCommand* inTextCommand)
{
	PDFUsedFont* currentFont = mGraphicStack.GetCurrentState().mFont;
	if(!currentFont)
	{
		TRACE_LOG("AbstractContentContext::WriteTextCommandWithEncoding, Cannot write text, no current font is defined");
		return eFailure;
	}

	GlyphUnicodeMappingList glyphsAndUnicode;
	EStatusCode encodingStatus = currentFont->TranslateStringToGlyphs(inUnicodeText,glyphsAndUnicode);

	// encoding returns false if was unable to encode some of the glyphs. will display as missing characters
	if(encodingStatus != eSuccess)
		TRACE_LOG("AbstractContextContext::WriteTextCommandWithEncoding, was unable to find glyphs for all characters, some will appear as missing");


	return WriteTextCommandWithDirectGlyphSelection(glyphsAndUnicode,inTextCommand);
}

class TjCommand : public ITextCommand
{
public:
	TjCommand(AbstractContentContext* inContext) {mContext = inContext;}

	virtual void WriteHexStringCommand(const string& inStringToWrite){mContext->TjHex(inStringToWrite);}
	virtual void WriteLiteralStringCommand(const string& inStringToWrite){mContext->Tj(inStringToWrite);}
private:
	AbstractContentContext* mContext;
};

EStatusCode AbstractContentContext::Tj(const wstring& inText)
{
	TjCommand command(this);
	return WriteTextCommandWithEncoding(inText,&command);
}

class QuoteCommand : public ITextCommand
{
public:
	QuoteCommand(AbstractContentContext* inContext) {mContext = inContext;}

	virtual void WriteHexStringCommand(const string& inStringToWrite){mContext->QuoteHex(inStringToWrite);}
	virtual void WriteLiteralStringCommand(const string& inStringToWrite){mContext->Quote(inStringToWrite);}
private:
	AbstractContentContext* mContext;
};

EStatusCode AbstractContentContext::Quote(const wstring& inText)
{
	QuoteCommand command(this);
	return WriteTextCommandWithEncoding(inText,&command);
}

class DoubleQuoteCommand : public ITextCommand
{
public:
	DoubleQuoteCommand(AbstractContentContext* inContext,
						double inWordSpacing,
						double inCharacterSpacing) {mContext = inContext;mWordSpacing = inWordSpacing;mCharacterSpacing = inCharacterSpacing;}

	virtual void WriteHexStringCommand(const string& inStringToWrite){mContext->DoubleQuoteHex(mWordSpacing,mCharacterSpacing,inStringToWrite);}
	virtual void WriteLiteralStringCommand(const string& inStringToWrite){mContext->DoubleQuote(mWordSpacing,mCharacterSpacing,inStringToWrite);}
private:
	AbstractContentContext* mContext;
	double mWordSpacing;
	double mCharacterSpacing;
};

EStatusCode AbstractContentContext::DoubleQuote(double inWordSpacing, double inCharacterSpacing, const wstring& inText)
{
	DoubleQuoteCommand command(this,inWordSpacing,inCharacterSpacing);
	return WriteTextCommandWithEncoding(inText,&command);
}

EStatusCode AbstractContentContext::TJ(const WStringOrDoubleList& inStringsAndSpacing)
{
	PDFUsedFont* currentFont = mGraphicStack.GetCurrentState().mFont;
	if(!currentFont)
	{
		TRACE_LOG("AbstractContentContext::TJ, Cannot write text, no current font is defined");
		return eFailure;
	}

	WStringOrDoubleList::const_iterator it = inStringsAndSpacing.begin();
	GlyphUnicodeMappingListOrDoubleList parameters;
	EStatusCode encodingStatus;
	
	for(; it != inStringsAndSpacing.end();++it)
	{
		if(it->IsDouble)
		{
			parameters.push_back(GlyphUnicodeMappingListOrDouble(it->DoubleValue));
		}
		else
		{
			GlyphUnicodeMappingList glyphsAndUnicode;
			encodingStatus = currentFont->TranslateStringToGlyphs(it->SomeValue,glyphsAndUnicode);

			// encoding returns false if was unable to encode some of the glyphs. will display as missing characters
			if(encodingStatus != eSuccess)
				TRACE_LOG("AbstractContextContext::TJ, was unable to find glyphs for all characters, some will appear as missing");
			parameters.push_back(GlyphUnicodeMappingListOrDouble(glyphsAndUnicode));
		}
	}

	return TJ(parameters);
}

EStatusCode AbstractContentContext::Tj(const GlyphUnicodeMappingList& inText)
{
	TjCommand command(this);
	return WriteTextCommandWithDirectGlyphSelection(inText,&command);
}

EStatusCode AbstractContentContext::WriteTextCommandWithDirectGlyphSelection(const GlyphUnicodeMappingList& inText,ITextCommand* inTextCommand)
{
	PDFUsedFont* currentFont = mGraphicStack.GetCurrentState().mFont;
	if(!currentFont)
	{
		TRACE_LOG("AbstractContentContext::WriteTextCommandWithDirectGlyphSelection, Cannot write text, no current font is defined");
		return eFailure;
	}

	ObjectIDType fontObjectID;
	UShortList encodedCharachtersList;
	bool writeAsCID;	

	if(currentFont->EncodeStringForShowing(inText,fontObjectID,encodedCharachtersList,writeAsCID) != eSuccess)
	{
		TRACE_LOG("AbstractcontextContext::WriteTextCommandWithDirectGlyphSelection, Unexepcted failure, Cannot encode characters");
		return eFailure;
	}
	

	// Write the font reference (only if required)
	std::string fontName = GetResourcesDictionary()->AddFontMapping(fontObjectID);

	if(mGraphicStack.GetCurrentState().mPlacedFontName != fontName)
		Tf(fontName,mGraphicStack.GetCurrentState().mFontSize);
	
	// Now write the string using the text command
	OutputStringBufferStream stringStream;
	char formattingBuffer[5];
	UShortList::iterator it = encodedCharachtersList.begin();
	if(writeAsCID)
	{
		for(;it!= encodedCharachtersList.end();++it)
		{
			SAFE_SPRINTF_2(formattingBuffer,5,"%02x%02x",((*it)>>8) & 0x00ff,(*it) & 0x00ff);
			stringStream.Write((const Byte*)formattingBuffer,4);
		}
		inTextCommand->WriteHexStringCommand(stringStream.ToString());
	}
	else
	{
		for(;it!= encodedCharachtersList.end();++it)
		{
			formattingBuffer[0] = (*it) & 0x00ff;
			stringStream.Write((const Byte*)formattingBuffer,1);
		}
		inTextCommand->WriteLiteralStringCommand(stringStream.ToString());	
	}
	return eSuccess;
}

EStatusCode AbstractContentContext::Quote(const GlyphUnicodeMappingList& inText)
{
	QuoteCommand command(this);
	return WriteTextCommandWithDirectGlyphSelection(inText,&command);
}

EStatusCode AbstractContentContext::DoubleQuote(double inWordSpacing, double inCharacterSpacing, const GlyphUnicodeMappingList& inText)
{
	DoubleQuoteCommand command(this,inWordSpacing,inCharacterSpacing);
	return WriteTextCommandWithDirectGlyphSelection(inText,&command);
}

EStatusCode AbstractContentContext::TJ(const GlyphUnicodeMappingListOrDoubleList& inStringsAndSpacing)
{
	PDFUsedFont* currentFont = mGraphicStack.GetCurrentState().mFont;
	if(!currentFont)
	{
		TRACE_LOG("AbstractContentContext::TJ, Cannot write text, no current font is defined");
		return eSuccess;
	}

	// TJ is a bit different. i want to encode all strings in the array to the same font, so that at most a single
	// Tf is used...and command may be written as is. for that we need to encode all strings at once...which requires
	// a slightly different algorithm

	// first, list all the strings, so that you can encode them
	GlyphUnicodeMappingListList stringsList;
	GlyphUnicodeMappingListOrDoubleList::const_iterator it = inStringsAndSpacing.begin();
	
	for(; it != inStringsAndSpacing.end(); ++it)
		if(!it->IsDouble)
			stringsList.push_back(it->SomeValue);

	// now, encode all strings at once

	ObjectIDType fontObjectID;
	UShortListList encodedCharachtersListsList;
	bool writeAsCID;	

	if(currentFont->EncodeStringsForShowing(stringsList,fontObjectID,encodedCharachtersListsList,writeAsCID)!= eSuccess)
	{
		TRACE_LOG("AbstractContentContext::TJ, Unexepcted failure, cannot include characters for writing final representation");
		return eFailure;
	}
	
	// status only returns if strings can be coded or not. so continue with writing regardless

	// Write the font reference (only if required)
	std::string fontName = GetResourcesDictionary()->AddFontMapping(fontObjectID);

	if(mGraphicStack.GetCurrentState().mPlacedFontName != fontName)
		Tf(fontName,mGraphicStack.GetCurrentState().mFontSize);
	
	// Now write the string using the text command
	OutputStringBufferStream stringStream;
	char formattingBuffer[5];
	StringOrDoubleList stringOrDoubleList;
	UShortListList::iterator itEncodedList = encodedCharachtersListsList.begin();
	UShortList::iterator itEncoded;

	if(writeAsCID)
	{
		for(it = inStringsAndSpacing.begin(); it != inStringsAndSpacing.end(); ++it)
		{
			if(it->IsDouble)
			{
				stringOrDoubleList.push_back(StringOrDouble(it->DoubleValue));
			}
			else
			{
				for(itEncoded = itEncodedList->begin();itEncoded!= itEncodedList->end();++itEncoded)
				{
					SAFE_SPRINTF_2(formattingBuffer,5,"%02x%02x",((*itEncoded)>>8) & 0x00ff,(*itEncoded) & 0x00ff);
					stringStream.Write((const Byte*)formattingBuffer,4);
				}
				stringOrDoubleList.push_back(StringOrDouble(stringStream.ToString()));
				stringStream.Reset();
				++itEncodedList;
			}
		}
		TJHex(stringOrDoubleList);
	}
	else
	{

		for(it = inStringsAndSpacing.begin(); it != inStringsAndSpacing.end(); ++it)
		{
			if(it->IsDouble)
			{
				stringOrDoubleList.push_back(StringOrDouble(it->DoubleValue));
			}
			else
			{
				for(itEncoded = itEncodedList->begin();itEncoded!= itEncodedList->end();++itEncoded)
				{
					formattingBuffer[0] = (*itEncoded) & 0x00ff;
					stringStream.Write((const Byte*)formattingBuffer,1);
				}
				stringOrDoubleList.push_back(StringOrDouble(stringStream.ToString()));
				stringStream.Reset();
				++itEncodedList;
			}
		}
		TJ(stringOrDoubleList);
	}
	return eSuccess;	
}
#include "AbstractContentContext.h"
#include "PDFStream.h"
#include "ResourcesDictionary.h"
#include "PDFImageXObject.h"

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
}

void AbstractContentContext::Q()
{
	RenewStreamConnection();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("Q");
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

void AbstractContentContext::Do(const string& inXObjectName,PDFImageXObject* inImageXObject)
{
	Do(inXObjectName);

	if(1 == inImageXObject->GetImageInformation().ColorComponentsCount)
		AssertProcsetAvailable(KProcsetImageB);
	else
		AssertProcsetAvailable(KProcsetImageC);
}
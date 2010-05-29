#include "PageContentContext.h"
#include "PDFPage.h"
#include "ObjectsContext.h"
#include "PDFStream.h"
#include "Trace.h"
#include "ResourcesDictionary.h"

PageContentContext::PageContentContext(PDFPage* inPageOfContext,ObjectsContext* inObjectsContext)
{
	mPageOfContext = inPageOfContext;
	mObjectsContext = inObjectsContext;
	mCurrentStream = NULL;
}

PageContentContext::~PageContentContext(void)
{
}

void PageContentContext::StartAStreamIfRequired()
{
	if(!mCurrentStream)
	{
		mCurrentStream = new PDFStream();
		mPrimitiveWriter.SetStreamForWriting(mCurrentStream->GetWriteStream());
	}
}

void PageContentContext::AssertProcsetAvailable(const string& inProcsetName)
{
	mPageOfContext->GetResourcesDictionary().AddProcsetResource(inProcsetName);	
}

static const string scAddRectangleToPath = "re";
void PageContentContext::re(double inLeft,double inBottom, double inWidth,double inHeight)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inLeft);
	mPrimitiveWriter.WriteDouble(inBottom);
	mPrimitiveWriter.WriteDouble(inWidth);
	mPrimitiveWriter.WriteDouble(inHeight);
	mPrimitiveWriter.WriteKeyword(scAddRectangleToPath);
}

static const string scFill = "f";
void PageContentContext::f()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword(scFill);
}

EStatusCode PageContentContext::FinalizeCurrentStream()
{
	if(mCurrentStream)
		return WriteCurrentStreamToMainStreamAndRelease();
	else
		return eSuccess;
}

EStatusCode PageContentContext::WriteCurrentStreamToMainStreamAndRelease()
{
	EStatusCode status;

	do
	{
		ObjectIDType streamObjectID = mObjectsContext->StartNewIndirectObject();
		mPageOfContext->AddContentStreamReference(streamObjectID);

		status = mObjectsContext->WritePDFStream(mCurrentStream);
		if(status != eSuccess)
		{
			TRACE_LOG("Failed to write page content stream");
			break;
		}

		mObjectsContext->EndIndirectObject();
		delete mCurrentStream;
		mCurrentStream = NULL;
	}while(false);

	return status;
}

void PageContentContext::S()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("S");
}

void PageContentContext::s()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("s");
}

void PageContentContext::F()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("F");
}

void PageContentContext::fStar()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("f*");
}

void PageContentContext::B()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("B");
}

void PageContentContext::BStar()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("B*");
}

void PageContentContext::b()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("b");
}

void PageContentContext::bStar()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("b*");
}

void PageContentContext::n()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("n");
}

void PageContentContext::m(double inX,double inY)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteKeyword("m");
}

void PageContentContext::l(double inX,double inY)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteKeyword("l");
}

void PageContentContext::c(	double inX1,double inY1, 
							double inX2, double inY2, 
							double inX3, double inY3)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX1);
	mPrimitiveWriter.WriteDouble(inY1);
	mPrimitiveWriter.WriteDouble(inX2);
	mPrimitiveWriter.WriteDouble(inY2);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("c");
}

void PageContentContext::v(	double inX2,double inY2, 
							double inX3, double inY3)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX2);
	mPrimitiveWriter.WriteDouble(inY2);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("v");
}

void PageContentContext::y(	double inX1,double inY1, 
							double inX3, double inY3)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inX1);
	mPrimitiveWriter.WriteDouble(inY1);
	mPrimitiveWriter.WriteDouble(inX3);
	mPrimitiveWriter.WriteDouble(inY3);
	mPrimitiveWriter.WriteKeyword("y");
}

void PageContentContext::h()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("h");
}

void PageContentContext::q()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("q");
}

void PageContentContext::Q()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("Q");
}

void PageContentContext::cm(double inA, double inB, double inC, double inD, double inE, double inF)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inA);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inD);
	mPrimitiveWriter.WriteDouble(inE);
	mPrimitiveWriter.WriteDouble(inF);
	mPrimitiveWriter.WriteKeyword("cm");
}

void PageContentContext::w(double inLineWidth)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inLineWidth);
	mPrimitiveWriter.WriteKeyword("w");
}

void PageContentContext::J(int inLineCapStyle)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inLineCapStyle);
	mPrimitiveWriter.WriteKeyword("J");
}

void PageContentContext::j(int inLineJoinStyle)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inLineJoinStyle);
	mPrimitiveWriter.WriteKeyword("j");
}

void PageContentContext::M(double inMiterLimit)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inMiterLimit);
	mPrimitiveWriter.WriteKeyword("M");
}

void PageContentContext::d(int* inDashArray, int inDashArrayLength,int inDashPhase)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.StartArray();
	for(int i=0;i<inDashArrayLength;++i)
		mPrimitiveWriter.WriteInteger(inDashArray[i]);
	mPrimitiveWriter.EndArray(eTokenSeparatorSpace);
	mPrimitiveWriter.WriteInteger(inDashPhase);
	mPrimitiveWriter.WriteKeyword("d");
}

void PageContentContext::ri(const string& inRenderingIntentName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inRenderingIntentName);
	mPrimitiveWriter.WriteKeyword("ri");
}

void PageContentContext::i(int inFlatness)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteInteger(inFlatness);
	mPrimitiveWriter.WriteKeyword("i");
}

void PageContentContext::gs(const string& inGraphicStateName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inGraphicStateName);
	mPrimitiveWriter.WriteKeyword("gs");
}


void PageContentContext::CS(const string& inColorSpaceName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inColorSpaceName);
	mPrimitiveWriter.WriteKeyword("CS");
}

void PageContentContext::cs(const string& inColorSpaceName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inColorSpaceName);
	mPrimitiveWriter.WriteKeyword("cs");
}

void PageContentContext::SC(double* inColorComponents, int inColorComponentsLength)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("SC");
}

void PageContentContext::SCN(double* inColorComponents, int inColorComponentsLength)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("SCN");
}

void PageContentContext::SCN(double* inColorComponents, int inColorComponentsLength,const string& inPatternName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteName(inPatternName);
	mPrimitiveWriter.WriteKeyword("SCN");
}

void PageContentContext::sc(double* inColorComponents, int inColorComponentsLength)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("sc");
}

void PageContentContext::scn(double* inColorComponents, int inColorComponentsLength)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteKeyword("scn");
}

void PageContentContext::scn(double* inColorComponents, int inColorComponentsLength,const string& inPatternName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	for(int i=0;i<inColorComponentsLength;++i)
		mPrimitiveWriter.WriteDouble(inColorComponents[i]);
	mPrimitiveWriter.WriteName(inPatternName);
	mPrimitiveWriter.WriteKeyword("scn");
}

void PageContentContext::G(double inGray)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inGray);
	mPrimitiveWriter.WriteKeyword("G");
}

void PageContentContext::g(double inGray)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inGray);
	mPrimitiveWriter.WriteKeyword("g");
}

void PageContentContext::RG(double inR,double inG,double inB)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inR);
	mPrimitiveWriter.WriteDouble(inG);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteKeyword("RG");
}

void PageContentContext::rg(double inR,double inG,double inB)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inR);
	mPrimitiveWriter.WriteDouble(inG);
	mPrimitiveWriter.WriteDouble(inB);
	mPrimitiveWriter.WriteKeyword("rg");
}

void PageContentContext::K(double inC,double inM,double inY,double inK)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inM);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteDouble(inK);
	mPrimitiveWriter.WriteKeyword("K");
}

void PageContentContext::k(double inC,double inM,double inY,double inK)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteDouble(inC);
	mPrimitiveWriter.WriteDouble(inM);
	mPrimitiveWriter.WriteDouble(inY);
	mPrimitiveWriter.WriteDouble(inK);
	mPrimitiveWriter.WriteKeyword("k");
}

void PageContentContext::W()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("W");
}

void PageContentContext::WStar()
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteKeyword("W*");
}

void PageContentContext::Do(const string& inXObjectName)
{
	StartAStreamIfRequired();
	AssertProcsetAvailable(KProcsetPDF);

	mPrimitiveWriter.WriteName(inXObjectName);	
	mPrimitiveWriter.WriteKeyword("Do");	
}

PDFStream* PageContentContext::GetCurrentPageContentStream()
{
	StartAStreamIfRequired();
	return mCurrentStream;	
}
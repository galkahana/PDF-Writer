#pragma once

#include "EStatusCode.h"
#include "PrimitiveObjectsWriter.h"
#include <string>

class PDFPage;
class ObjectsContext;
class PDFStream;

class PageContentContext
{
public:
	PageContentContext(PDFPage* inPageOfContext,ObjectsContext* inObjectsContext);
	virtual ~PageContentContext(void);

	// Finish writing a current stream, if existsm and flush to the main PDF stream
	EStatusCode FinalizeCurrentStream();

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
	void Q();
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


	// Extensibility method, retrieves the current content stream for writing. if one does not exist - creates it.
	PDFStream* GetCurrentPageContentStream();

private:
	PDFPage* mPageOfContext;
	ObjectsContext* mObjectsContext;
	PDFStream* mCurrentStream;
	PrimitiveObjectsWriter mPrimitiveWriter;

	EStatusCode WriteCurrentStreamToMainStreamAndRelease();
	void AssertProcsetAvailable(const string& inProcsetName);
	void StartAStreamIfRequired();
};

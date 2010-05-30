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
#include <string>

class ObjectsContext;
class PDFStream;
class ResourcesDictionary;

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

protected:

	// Derived classes should use this method to update the stream for writing
	void SetPDFStreamForWrite(PDFStream* inStream);

private:
	// Derived classes should use this method to retrive the content resource dictionary, for updating procsets 'n such
	virtual ResourcesDictionary* GetResourcesDictionary() = 0;
	// Derived classes should optionally use this method if the stream needs updating (use calls to SetPDFStreamForWrite for this purpose)
	virtual void RenewStreamConnection() {};

	PrimitiveObjectsWriter mPrimitiveWriter;

	void AssertProcsetAvailable(const string& inProcsetName);

};

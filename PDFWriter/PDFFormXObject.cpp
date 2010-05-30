#include "PDFFormXObject.h"
#include "PDFStream.h"
#include "XObjectContentContext.h"

PDFFormXObject::PDFFormXObject(ObjectIDType inXObjectID)
{
	mXObjectID = inXObjectID;
	mContentStream = new PDFStream();
	mContentContext = new XObjectContentContext(this);
	SetMatrix(1,0,0,1,0,0);
	
}

PDFFormXObject::~PDFFormXObject(void)
{
	delete mContentStream;
	delete mContentContext;
}

void PDFFormXObject::SetBoundingBox(const PDFRectangle& inBoundingBox)
{
	mBBox = inBoundingBox;
}

const PDFRectangle& PDFFormXObject::GetBoundingBox() const
{
	return mBBox;
}

ObjectIDType PDFFormXObject::GetObjectID()
{
	return mXObjectID;
}

ResourcesDictionary& PDFFormXObject::GetResourcesDictionary()
{
	return mResources;
}

PDFStream* PDFFormXObject::GetContentStream()
{
	return mContentStream;
}

XObjectContentContext* PDFFormXObject::GetContentContext()
{
	return mContentContext;
}

void PDFFormXObject::SetMatrix(double inA,double inB,double inC,double inD,double inE,double inF)
{
	mMatrix[0] = inA;
	mMatrix[1] = inB;
	mMatrix[2] = inC;
	mMatrix[3] = inD;
	mMatrix[4] = inE;
	mMatrix[5] = inF;
}

const double* PDFFormXObject::GetMatrix() const
{
	return mMatrix;
}
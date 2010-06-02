#pragma once

#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"
#include "ResourcesDictionary.h"

class PDFStream;
class XObjectContentContext;
class ObjectsContext;

class PDFFormXObject
{
public:
	PDFFormXObject(ObjectsContext* inObjectsContext);
	~PDFFormXObject(void);

	void SetBoundingBox(const PDFRectangle& inBoundingBox);
	const PDFRectangle& GetBoundingBox() const;

	void SetMatrix(double inA,double inB,double inC,double inD,double inE,double inF);
	const double* GetMatrix() const;

	ObjectIDType GetObjectID();

	ResourcesDictionary& GetResourcesDictionary();
	XObjectContentContext* GetContentContext();
	PDFStream* GetContentStream();
	
private:

	double mMatrix[6];
	ObjectIDType mXObjectID;
	PDFRectangle mBBox;
	ResourcesDictionary mResources;
	PDFStream* mContentStream;
	XObjectContentContext* mContentContext;
};

#pragma once
#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"

#include <list>
#include <utility>

class PDFFormXObject;
class PDFPage;

using namespace std;

typedef pair<unsigned long,unsigned long> ULongAndULong;
typedef list<ULongAndULong> ULongAndULongList;
typedef list<PDFFormXObject*> PDFFormXObjectList;
typedef pair<EStatusCode,PDFFormXObjectList> EStatusCodeAndPDFFormXObjectList;

typedef list<ObjectIDType> ObjectIDTypeList;
typedef pair<EStatusCode,ObjectIDTypeList> EStatusCodeAndObjectIDTypeList;
typedef pair<EStatusCode,ObjectIDType> EStatusCodeAndObjectIDType;

/*
	PDFPageRange is to be used as a method to select pages ranges. quite simply it's either "All" or "Specific".
	The choice is represented by mType member.
	With specific you get to describe the particular ranges. the ranges are a strictly ascending list of pairs, where each
	pair marks an inclusive range of pages. e.g good paris are : [1,3], [4,6], [7,7].  indexes should be zero based.
*/
struct PDFPageRange
{
	enum ERangeType
	{
		eRangeTypeAll,
		eRangeTypeSpecific
	};

	PDFPageRange() {mType = eRangeTypeAll;}

	ERangeType mType;
	ULongAndULongList mSpecificRanges;
};

/*
	EPDFPageBox enumerates PDF boxes, to select from, when in need.
*/
enum EPDFPageBox
{
	ePDFPageBoxMediaBox,
	ePDFPageBoxCropBox,
	ePDFPageBoxBleedBox,
	ePDFPageBoxTrimBox,
	ePDFPageBoxArtBox
};
#pragma once

#include "SingleValueContainerIterator.h"
#include "MapIterator.h"
#include "ObjectsBasicTypes.h"

#include <set>
#include <map>
#include <string>

using namespace std;

// Predefined procset names
static const string KProcsetPDF = "PDF";
static const string KProcsetText = "Text";
static const string KProcsetImageB = "ImageB";
static const string KProcsetImageC = "ImageC";
static const string KProcsetImageI = "ImageI";


typedef set<string> StringSet;
typedef map<ObjectIDType,string> ObjectIDTypeToStringMap;

class ResourcesDictionary
{
public:
	ResourcesDictionary(void);
	virtual ~ResourcesDictionary(void);

	void AddProcsetResource(const string& inResourceName);
	int GetProcsetsCount();
	SingleValueContainerIterator<StringSet> GetProcesetsIterator();

	// Use AddFormXObjectMapping to use a form XObject in a content stream [page or xobject].
	// AddFromXObjectMapping(inFormXObjectID) returns a string name that you can use for 'Do' calls
	string AddFormXObjectMapping(ObjectIDType inFormXObjectID);
	// AddFormXObjectMapping(inFormXObjectID,inFormXObjectName) should be used when the mechanism
	// for determining XObject names is external to ResourcesDictionary. it is highly recommended
	// that if One overload is used, it is used any time the particular resource dictionary is handled - this will avoid
	// collisions in naming between the internal and external mechanism.
	void AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName);
	
	int GetXObjectsCount();
	MapIterator<ObjectIDTypeToStringMap> GetFormXObjectsIterator();
	
private:

	StringSet mProcsets;
	ObjectIDTypeToStringMap mFormXObjects;
	
};


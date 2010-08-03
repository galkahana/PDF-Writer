#pragma once

#include "SingleValueContainerIterator.h"
#include "MapIterator.h"
#include "ObjectsBasicTypes.h"

#include <set>
#include <map>
#include <string>

using namespace std;

typedef set<string> StringSet;
typedef map<ObjectIDType,string> ObjectIDTypeToStringMap;

class PDFImageXObject;

class ResourcesDictionary
{
public:
	ResourcesDictionary(void);
	virtual ~ResourcesDictionary(void);

	void AddProcsetResource(const string& inResourceName);
	int GetProcsetsCount();
	SingleValueContainerIterator<StringSet> GetProcesetsIterator();

	int GetXObjectsCount();

	// Use AddFormXObjectMapping to use a form XObject in a content stream [page or xobject].
	// AddFromXObjectMapping(inFormXObjectID) returns a string name that you can use for 'Do' calls
	string AddFormXObjectMapping(ObjectIDType inFormXObjectID);
	// AddFormXObjectMapping(inFormXObjectID,inFormXObjectName) should be used when the mechanism
	// for determining XObject names is external to ResourcesDictionary. it is highly recommended
	// that if One overload is used, it is used any time the particular resource dictionary is handled - this will avoid
	// collisions in naming between the internal and external mechanism.
	void AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName);
	
	int GetFormXObjectsCount();
	MapIterator<ObjectIDTypeToStringMap> GetFormXObjectsIterator();

	// images. same idea as forms. note that image define resources that should
	// be added to the container resources dictionary
	string AddImageXObjectMapping(PDFImageXObject* inImageXObject);
	void AddImageXObjectMapping(PDFImageXObject* inImageXObject, const string& inImageXObjectName);

	// images registration without the automatic addition of image resources to the container resources dictionary
	string AddImageXObjectMapping(ObjectIDType inImageXObjectID);
	void AddImageXObjectMapping(ObjectIDType inImageXObjectID, const string& inImageXObjectName);


	int GetImageXObjectsCount();
	MapIterator<ObjectIDTypeToStringMap> GetImageXObjectsIterator();

	// ExtGStates. 
	string AddExtGStateMapping(ObjectIDType inExtGStateID);
	void AddExtGStateMapping(ObjectIDType inExtGStateID, const string& inExtGStateName);
	int GetExtGStatesCount();
	MapIterator<ObjectIDTypeToStringMap> GetExtGStatesIterator();

private:

	StringSet mProcsets;
	ObjectIDTypeToStringMap mFormXObjects;
	ObjectIDTypeToStringMap mImageXObjects;
	ObjectIDTypeToStringMap mExtGStates;
	

	void AddImageXObjectMappingWithName(PDFImageXObject* inImageXObject, const string& inImageXObjectName);

};


/*
   Source File : ResourcesDictionary.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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
	SingleValueContainerIterator<StringSet> GetProcesetsIterator();

	// ExtGStates. 
	string AddExtGStateMapping(ObjectIDType inExtGStateID);
	void AddExtGStateMapping(ObjectIDType inExtGStateID, const string& inExtGStateName);
	MapIterator<ObjectIDTypeToStringMap> GetExtGStatesIterator();


	// Fonts.
	string AddFontMapping(ObjectIDType inFontObjectID);
	void AddFontMapping(ObjectIDType inFontObjectID,const string& inFontObjectName);
	MapIterator<ObjectIDTypeToStringMap> GetFontsIterator();


	// Color space
	string AddColorSpaceMapping(ObjectIDType inColorspaceID);
	MapIterator<ObjectIDTypeToStringMap> GetColorSpacesIterator();

	// Patterns
	string AddPatternMapping(ObjectIDType inPatternID);
	MapIterator<ObjectIDTypeToStringMap> GetPatternsIterator();

	// Properties
	string AddPropertyMapping(ObjectIDType inPropertyID);
	MapIterator<ObjectIDTypeToStringMap> GetPropertiesIterator();
	

	// XObjects
	string AddXObjectMapping(ObjectIDType inXObjectID);
	MapIterator<ObjectIDTypeToStringMap> GetXObjectsIterator();

	// Use AddFormXObjectMapping to use a form XObject in a content stream [page or xobject].
	// AddFromXObjectMapping(inFormXObjectID) returns a string name that you can use for 'Do' calls
	string AddFormXObjectMapping(ObjectIDType inFormXObjectID);
	// AddFormXObjectMapping(inFormXObjectID,inFormXObjectName) should be used when the mechanism
	// for determining XObject names is external to ResourcesDictionary. it is highly recommended
	// that if One overload is used, it is used any time the particular resource dictionary is handled - this will avoid
	// collisions in naming between the internal and external mechanism.
	void AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName);
    
	// images. same idea as forms. note that image define resources that should
	// be added to the container resources dictionary
	string AddImageXObjectMapping(PDFImageXObject* inImageXObject);
	void AddImageXObjectMapping(PDFImageXObject* inImageXObject, const string& inImageXObjectName);
    
	// images registration without the automatic addition of image resources to the container resources dictionary
	string AddImageXObjectMapping(ObjectIDType inImageXObjectID);
	void AddImageXObjectMapping(ObjectIDType inImageXObjectID, const string& inImageXObjectName);
    
	// Shading
	string AddShadingMapping(ObjectIDType inShadingID);
	MapIterator<ObjectIDTypeToStringMap> GetShadingsIterator();

private:

	StringSet mProcsets;
    unsigned long mFormXObjectsCount;
    unsigned long mImageXObjectsCount;
    unsigned long mGenericXObjectsCount;
	ObjectIDTypeToStringMap mExtGStates;
    unsigned long mExtGStatesCount;
	ObjectIDTypeToStringMap mFonts;
	unsigned long mFontsCount;
	ObjectIDTypeToStringMap mColorSpaces;
    unsigned long mColorSpacesCount;
	ObjectIDTypeToStringMap mPatterns;
    unsigned long mPatternsCount;
	ObjectIDTypeToStringMap mProperties;
    unsigned long mPropertiesCount;
	ObjectIDTypeToStringMap mXObjects;
	ObjectIDTypeToStringMap mShading;
    unsigned long mShadingCount;

	void AddImageXObjectMappingWithName(PDFImageXObject* inImageXObject, const string& inImageXObjectName);

};


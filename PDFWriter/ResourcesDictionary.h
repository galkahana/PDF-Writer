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

	string AddFontMapping(ObjectIDType inFontObjectID);
	void AddFontMapping(ObjectIDType inFontObjectID,const string& inFontObjectName);


	int GetImageXObjectsCount();
	MapIterator<ObjectIDTypeToStringMap> GetImageXObjectsIterator();

	// ExtGStates. 
	string AddExtGStateMapping(ObjectIDType inExtGStateID);
	void AddExtGStateMapping(ObjectIDType inExtGStateID, const string& inExtGStateName);
	int GetExtGStatesCount();
	MapIterator<ObjectIDTypeToStringMap> GetExtGStatesIterator();


	// Fonts.
	int GetFontsCount();
	MapIterator<ObjectIDTypeToStringMap> GetFontsIterator();


	// Color space
	string AddColorSpaceMapping(ObjectIDType inColorspaceID);
	int GetColorSpacesCount();
	MapIterator<ObjectIDTypeToStringMap> GetColorSpacesIterator();

	// Patterns
	string AddPatternMapping(ObjectIDType inPatternID);
	int GetPatternsCount();
	MapIterator<ObjectIDTypeToStringMap> GetPatternsIterator();

	// Properties
	string AddPropertyMapping(ObjectIDType inPropertyID);
	int GetPropertiesCount();
	MapIterator<ObjectIDTypeToStringMap> GetPropertiesIterator();
	

	// Generic XObjects
	string AddGenericXObjectMapping(ObjectIDType inXObjectID);
	int GetGenericXObjectsCount();
	MapIterator<ObjectIDTypeToStringMap> GetGenericXObjectsIterator();

	// Shading
	string AddShadingMapping(ObjectIDType inShadingID);
	int GetShadingsCount();
	MapIterator<ObjectIDTypeToStringMap> GetShadingsIterator();

private:

	StringSet mProcsets;
	ObjectIDTypeToStringMap mFormXObjects;
	ObjectIDTypeToStringMap mImageXObjects;
	ObjectIDTypeToStringMap mExtGStates;
	ObjectIDTypeToStringMap mFonts;
	
	ObjectIDTypeToStringMap mColorSpaces;
	ObjectIDTypeToStringMap mPatterns;
	ObjectIDTypeToStringMap mProperties;
	ObjectIDTypeToStringMap mGenericXObjects;
	ObjectIDTypeToStringMap mShading;

	void AddImageXObjectMappingWithName(PDFImageXObject* inImageXObject, const string& inImageXObjectName);

};


/*
   Source File : ResourcesDictionary.cpp


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
#include "ResourcesDictionary.h"
#include "BoxingBase.h"
#include "PDFImageXObject.h"

ResourcesDictionary::ResourcesDictionary(void)
{
}

ResourcesDictionary::~ResourcesDictionary(void)
{
}

void ResourcesDictionary::AddProcsetResource(const string& inResourceName)
{
	if(mProcsets.find(inResourceName) == mProcsets.end())
		mProcsets.insert(inResourceName);
}

int ResourcesDictionary::GetProcsetsCount()
{
	return (int)mProcsets.size();
}

SingleValueContainerIterator<StringSet> ResourcesDictionary::GetProcesetsIterator()
{
	return SingleValueContainerIterator<StringSet>(mProcsets);
}

static const string scFM = "Fm";
string ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mFormXObjects.find(inFormXObjectID);

	if(it == mFormXObjects.end())
	{
		string newName = scFM + Int((int)mFormXObjects.size()+1).ToString();
		mFormXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mFormXObjects.find(inFormXObjectID);

	if(it == mFormXObjects.end())
		mFormXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,inFormXObjectName));
	else
		it->second = inFormXObjectName;
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetFormXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mFormXObjects);
}

int ResourcesDictionary::GetFormXObjectsCount()
{
	return (int)mFormXObjects.size();
}

static const string scIM = "Im";
string ResourcesDictionary::AddImageXObjectMapping(PDFImageXObject* inImageXObject)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObject->GetImageObjectID());

	if(it == mImageXObjects.end())
	{
		string newName = scIM + Int((int)mImageXObjects.size()+1).ToString();
		AddImageXObjectMappingWithName(inImageXObject,newName);
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddImageXObjectMappingWithName(PDFImageXObject* inImageXObject, const string& inImageXObjectName)
{
	mImageXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObject->GetImageObjectID(),inImageXObjectName));

	StringList::const_iterator it = inImageXObject->GetRequiredProcsetResourceNames().begin();
	for(; it != inImageXObject->GetRequiredProcsetResourceNames().end();++it)
	{
		if(it->size() > 0)
			AddProcsetResource(*it);
	}
}


void ResourcesDictionary::AddImageXObjectMapping(PDFImageXObject* inImageXObject, const string& inImageXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObject->GetImageObjectID());

	if(it == mImageXObjects.end())
		AddImageXObjectMappingWithName(inImageXObject,inImageXObjectName);
	else
		it->second = inImageXObjectName;
}

int ResourcesDictionary::GetImageXObjectsCount()
{
	return (int)mImageXObjects.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetImageXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mImageXObjects);
}

int ResourcesDictionary::GetXObjectsCount()
{
	return int(mImageXObjects.size() + mFormXObjects.size() + mGenericXObjects.size());
}

static const string scGS = "GS";
string ResourcesDictionary::AddExtGStateMapping(ObjectIDType inExtGStateID)
{
	ObjectIDTypeToStringMap::iterator it = mExtGStates.find(inExtGStateID);

	if(it == mExtGStates.end())
	{
		string newName = scGS + Int((int)mExtGStates.size()+1).ToString();
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddExtGStateMapping(ObjectIDType inExtGStateID, const string& inExtGStateName)
{
	ObjectIDTypeToStringMap::iterator it = mExtGStates.find(inExtGStateID);

	if(it == mExtGStates.end())
		mExtGStates.insert(ObjectIDTypeToStringMap::value_type(inExtGStateID,inExtGStateName));
	else
		it->second = inExtGStateName;
}

int ResourcesDictionary::GetExtGStatesCount()
{
	return (int)mExtGStates.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetExtGStatesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mExtGStates);
}

string ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObjectID);

	if(it == mImageXObjects.end())
	{
		string newName = scIM + Int((int)mImageXObjects.size()+1).ToString();
		mImageXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID, const string& inImageXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mImageXObjects.find(inImageXObjectID);

	if(it == mImageXObjects.end())
		mImageXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,inImageXObjectName));
	else
		it->second = inImageXObjectName;
}

static const string scFN = "FN";
string ResourcesDictionary::AddFontMapping(ObjectIDType inFontObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mFonts.find(inFontObjectID);

	if(it == mFonts.end())
	{
		string newName = scFN + Int((int)mFonts.size()+1).ToString();
		mFonts.insert(ObjectIDTypeToStringMap::value_type(inFontObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

void ResourcesDictionary::AddFontMapping(ObjectIDType inFontObjectID,const string& inFontObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mFonts.find(inFontObjectID);

	if(it == mFonts.end())
		mFonts.insert(ObjectIDTypeToStringMap::value_type(inFontObjectID,inFontObjectName));
	else
		it->second = inFontObjectName;
}

int ResourcesDictionary::GetFontsCount()
{
	return (int)mFonts.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetFontsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mFonts);
}

// Color space
static const string scCS = "CS";
string ResourcesDictionary::AddColorSpaceMapping(ObjectIDType inColorspaceID)
{
	ObjectIDTypeToStringMap::iterator it = mColorSpaces.find(inColorspaceID);

	if(it == mColorSpaces.end())
	{
		string newName = scCS + Int((int)mColorSpaces.size()+1).ToString();
		mColorSpaces.insert(ObjectIDTypeToStringMap::value_type(inColorspaceID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

int ResourcesDictionary::GetColorSpacesCount()
{
	return (int)mColorSpaces.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetColorSpacesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mColorSpaces);
}

// Patterns
static const string scPT = "PT";
string ResourcesDictionary::AddPatternMapping(ObjectIDType inPatternID)
{
	ObjectIDTypeToStringMap::iterator it = mPatterns.find(inPatternID);

	if(it == mPatterns.end())
	{
		string newName = scPT + Int((int)mPatterns.size()+1).ToString();
		mPatterns.insert(ObjectIDTypeToStringMap::value_type(inPatternID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

int ResourcesDictionary::GetPatternsCount()
{
	return (int)mPatterns.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetPatternsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mPatterns);
}


// Properties
static const string scPP = "PP";
string ResourcesDictionary::AddPropertyMapping(ObjectIDType inPropertyID)
{
	ObjectIDTypeToStringMap::iterator it = mProperties.find(inPropertyID);

	if(it == mProperties.end())
	{
		string newName = scPP + Int((int)mProperties.size()+1).ToString();
		mProperties.insert(ObjectIDTypeToStringMap::value_type(inPropertyID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

int ResourcesDictionary::GetPropertiesCount()
{
	return (int)mProperties.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetPropertiesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mProperties);
}

// Generic XObjects
static const string scXO = "XO";
string ResourcesDictionary::AddGenericXObjectMapping(ObjectIDType inXObjectID)
{
	ObjectIDTypeToStringMap::iterator it = mGenericXObjects.find(inXObjectID);

	if(it == mGenericXObjects.end())
	{
		string newName = scXO + Int((int)mGenericXObjects.size()+1).ToString();
		mGenericXObjects.insert(ObjectIDTypeToStringMap::value_type(inXObjectID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

int ResourcesDictionary::GetGenericXObjectsCount()
{
	return (int)mGenericXObjects.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetGenericXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mGenericXObjects);
}

// Shading
static const string scSH = "SH";
string ResourcesDictionary::AddShadingMapping(ObjectIDType inShadingID)
{
	ObjectIDTypeToStringMap::iterator it = mShading.find(inShadingID);

	if(it == mShading.end())
	{
		string newName = scSH + Int((int)mShading.size()+1).ToString();
		mShading.insert(ObjectIDTypeToStringMap::value_type(inShadingID,newName));
		return newName;
	}
	else
	{
		return it->second;
	}
}

int ResourcesDictionary::GetShadingsCount()
{
	return (int)mShading.size();
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetShadingsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mShading);
}

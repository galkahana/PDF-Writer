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

typedef BoxingBaseWithRW<unsigned long> ULong;

ResourcesDictionary::ResourcesDictionary(void)
{
    mImageXObjectsCount = 0;
    mFormXObjectsCount = 0;
    mGenericXObjectsCount = 0;
    mExtGStatesCount = 0;
	mFontsCount = 0;
    mColorSpacesCount = 0;
    mPatternsCount = 0;
    mPropertiesCount = 0;
    mShadingCount = 0;

}

ResourcesDictionary::~ResourcesDictionary(void)
{
}

void ResourcesDictionary::AddProcsetResource(const string& inResourceName)
{
	if(mProcsets.find(inResourceName) == mProcsets.end())
		mProcsets.insert(inResourceName);
}

SingleValueContainerIterator<StringSet> ResourcesDictionary::GetProcesetsIterator()
{
	return SingleValueContainerIterator<StringSet>(mProcsets);
}

static const string scFM = "Fm";
string ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID)
{
    if(inFormXObjectID == 0)
    {
        string newName = scFM + ULong(mFormXObjectsCount+1).ToString();
        ++mFormXObjectsCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mXObjects.find(inFormXObjectID);
        
        if(it == mXObjects.end())
        {
            string newName = scFM + ULong(mFormXObjectsCount+1).ToString();
            ++mFormXObjectsCount;
            it = mXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,newName)).first;
        }
        return it->second;
    }
}

void ResourcesDictionary::AddFormXObjectMapping(ObjectIDType inFormXObjectID,const string& inFormXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mXObjects.find(inFormXObjectID);

	if(it == mXObjects.end())
		mXObjects.insert(ObjectIDTypeToStringMap::value_type(inFormXObjectID,inFormXObjectName));
	else
		it->second = inFormXObjectName;
}

static const string scIM = "Im";
string ResourcesDictionary::AddImageXObjectMapping(PDFImageXObject* inImageXObject)
{
	ObjectIDTypeToStringMap::iterator it = mXObjects.find(inImageXObject->GetImageObjectID());

	if(it == mXObjects.end())
	{
		string newName = scIM + ULong(mImageXObjectsCount+1).ToString();
        ++mImageXObjectsCount;
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
	mXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObject->GetImageObjectID(),inImageXObjectName));
	
	StringList::const_iterator it = inImageXObject->GetRequiredProcsetResourceNames().begin();
	for(; it != inImageXObject->GetRequiredProcsetResourceNames().end();++it)
	{
		if(it->size() > 0)
			AddProcsetResource(*it);
	}
}


void ResourcesDictionary::AddImageXObjectMapping(PDFImageXObject* inImageXObject, const string& inImageXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mXObjects.find(inImageXObject->GetImageObjectID());

	if(it == mXObjects.end())
		AddImageXObjectMappingWithName(inImageXObject,inImageXObjectName);
	else
		it->second = inImageXObjectName;
}

static const string scGS = "GS";
string ResourcesDictionary::AddExtGStateMapping(ObjectIDType inExtGStateID)
{
    if(inExtGStateID == 0)
    {
        string newName = scGS + ULong(mExtGStatesCount+1).ToString();
        ++mExtGStatesCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mXObjects.find(inExtGStateID);
        
        if(it == mXObjects.end())
        {
            string newName = scGS + ULong(mExtGStatesCount+1).ToString();
            ++mExtGStatesCount;
            it = mExtGStates.insert(ObjectIDTypeToStringMap::value_type(inExtGStateID,newName)).first;
        }
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


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetExtGStatesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mExtGStates);
}

string ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID)
{
    if(inImageXObjectID == 0)
    {
        string newName = scIM + ULong(mImageXObjectsCount+1).ToString();
        ++mImageXObjectsCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mXObjects.find(inImageXObjectID);
        
        if(it == mXObjects.end())
        {
            string newName = scIM + ULong(mImageXObjectsCount+1).ToString();
            ++mImageXObjectsCount;
            it = mXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,newName)).first;
        }
        return it->second;
    }
}

void ResourcesDictionary::AddImageXObjectMapping(ObjectIDType inImageXObjectID, const string& inImageXObjectName)
{
	ObjectIDTypeToStringMap::iterator it = mXObjects.find(inImageXObjectID);

	if(it == mXObjects.end())
		mXObjects.insert(ObjectIDTypeToStringMap::value_type(inImageXObjectID,inImageXObjectName));
	else
		it->second = inImageXObjectName;
}

static const string scFN = "FN";
string ResourcesDictionary::AddFontMapping(ObjectIDType inFontObjectID)
{
    if(inFontObjectID == 0)
    {
        string newName = scFN + ULong(mFontsCount+1).ToString();
        ++mFontsCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mFonts.find(inFontObjectID);
        
        if(it == mFonts.end())
        {
            string newName = scFN + ULong(mFontsCount+1).ToString();
            ++mFontsCount;
            it = mFonts.insert(ObjectIDTypeToStringMap::value_type(inFontObjectID,newName)).first;
        }
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


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetFontsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mFonts);
}

// Color space
static const string scCS = "CS";
string ResourcesDictionary::AddColorSpaceMapping(ObjectIDType inColorspaceID)
{
    if(inColorspaceID == 0)
    {
        string newName = scCS + ULong(mColorSpacesCount+1).ToString();
        ++mColorSpacesCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mColorSpaces.find(inColorspaceID);
        
        if(it == mColorSpaces.end())
        {
            string newName = scCS + ULong(mColorSpacesCount+1).ToString();
            ++mColorSpacesCount;
            it = mColorSpaces.insert(ObjectIDTypeToStringMap::value_type(inColorspaceID,newName)).first;
        }
        return it->second;
    }
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetColorSpacesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mColorSpaces);
}

// Patterns
static const string scPT = "PT";
string ResourcesDictionary::AddPatternMapping(ObjectIDType inPatternID)
{
    if(inPatternID == 0)
    {
        string newName = scPT + ULong(mPatternsCount+1).ToString();
        ++mPatternsCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mPatterns.find(inPatternID);
        
        if(it == mPatterns.end())
        {
            string newName = scPT + ULong(mPatternsCount+1).ToString();
            ++mPatternsCount;
            it = mPatterns.insert(ObjectIDTypeToStringMap::value_type(inPatternID,newName)).first;
        }
        return it->second;
    }
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetPatternsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mPatterns);
}


// Properties
static const string scPP = "PP";
string ResourcesDictionary::AddPropertyMapping(ObjectIDType inPropertyID)
{
    if(inPropertyID == 0)
    {
        string newName = scPP + ULong(mPropertiesCount+1).ToString();
        ++mPropertiesCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mProperties.find(inPropertyID);
        
        if(it == mProperties.end())
        {
            string newName = scPP + ULong(mPropertiesCount+1).ToString();
            ++mPropertiesCount;
            it = mProperties.insert(ObjectIDTypeToStringMap::value_type(inPropertyID,newName)).first;
        }
        return it->second;
    }
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetPropertiesIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mProperties);
}

// Generic XObjects
static const string scXO = "XO";
string ResourcesDictionary::AddXObjectMapping(ObjectIDType inXObjectID)
{
    if(inXObjectID == 0)
    {
        string newName = scXO + ULong(mGenericXObjectsCount+1).ToString();
        ++mGenericXObjectsCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mXObjects.find(inXObjectID);
        
        if(it == mXObjects.end())
        {
            string newName = scXO + ULong(mGenericXObjectsCount+1).ToString();
            ++mGenericXObjectsCount;
            it = mXObjects.insert(ObjectIDTypeToStringMap::value_type(inXObjectID,newName)).first;
        }
        return it->second;
    }
}


MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetXObjectsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mXObjects);
}

// Shading
static const string scSH = "SH";
string ResourcesDictionary::AddShadingMapping(ObjectIDType inShadingID)
{
    if(inShadingID == 0)
    {
        string newName = scSH + ULong(mShadingCount+1).ToString();
        ++mShadingCount;
        return newName;
    }
    else 
    {
        ObjectIDTypeToStringMap::iterator it = mShading.find(inShadingID);
        
        if(it == mShading.end())
        {
            string newName = scSH + ULong(mShadingCount+1).ToString();
            ++mShadingCount;
            it = mShading.insert(ObjectIDTypeToStringMap::value_type(inShadingID,newName)).first;
        }
        return it->second;
    }
}

MapIterator<ObjectIDTypeToStringMap> ResourcesDictionary::GetShadingsIterator()
{
	return MapIterator<ObjectIDTypeToStringMap>(mShading);
}

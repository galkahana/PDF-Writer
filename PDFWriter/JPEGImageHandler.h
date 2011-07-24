/*
   Source File : JPEGImageHandler.h


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

#include "JPEGImageInformation.h"
#include "ObjectsBasicTypes.h"

#include <map>
#include <string>
#include <utility>
#include <set>

using namespace std;

class ObjectsContext;
class PDFImageXObject;
class IDocumentContextExtender;
class PDFFormXObject;
class DocumentContext;
class IByteReaderWithPosition;

typedef map<wstring,JPEGImageInformation> WStringToJPEGImageInformationMap;
typedef pair<bool,JPEGImageInformation> BoolAndJPEGImageInformation;
typedef pair<double,double> DoubleAndDoublePair;
typedef set<IDocumentContextExtender*> IDocumentContextExtenderSet;


class JPEGImageHandler
{
public:
	JPEGImageHandler();
	~JPEGImageHandler(void);

	// use this for retrieving image information for JPEG (useful for deciphering JPG dimensions tags)
	BoolAndJPEGImageInformation RetrieveImageInformation(const wstring& inJPGFilePath);

	// DocumentContext::CreateImageXObjectFromJPGFile are equivelent
	PDFImageXObject* CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath);
	PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
	PDFImageXObject* CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID);
	PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID);
	
	// will return form XObject, which will include the xobject at it's size
	PDFFormXObject* CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath);
	PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
	PDFFormXObject* CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inFormXObjectID);
	PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID);

	void SetOperationsContexts(DocumentContext* inDocumentContext,ObjectsContext* inObjectsContext);
	void AddDocumentContextExtender(IDocumentContextExtender* inExtender);
	void RemoveDocumentContextExtender(IDocumentContextExtender* inExtender);

private:
	JPEGImageInformation mNullInformation;
	WStringToJPEGImageInformationMap mImagesInformationMap;
	ObjectsContext* mObjectsContext;
	DocumentContext* mDocumentContext;
	IDocumentContextExtenderSet mExtenders;

	PDFImageXObject* CreateAndWriteImageXObjectFromJPGInformation(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID, const JPEGImageInformation& inJPGImageInformation);
	PDFImageXObject* CreateAndWriteImageXObjectFromJPGInformation(IByteReaderWithPosition* inJPGImageStream,ObjectIDType inImageXObjectID, const JPEGImageInformation& inJPGImageInformation);
	PDFFormXObject* CreateImageFormXObjectFromImageXObject(PDFImageXObject* inImageXObject,ObjectIDType inFormXObjectID, const JPEGImageInformation& inJPGImageInformation);
	DoubleAndDoublePair GetImageDimensions(const JPEGImageInformation& inJPGImageInformation);

};

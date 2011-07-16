/*
   Source File : PDFDocumentHandler.h


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

#include "PDFEmbedParameterTypes.h"
#include "PDFParser.h"
#include "PDFRectangle.h"
#include "InputFile.h"
#include "ObjectsBasicTypes.h"
#include "ETokenSeparator.h"
#include "DocumentContextExtenderAdapter.h"
#include "MapIterator.h"

#include <map>
#include <list>
#include <set>

class DocumentContext;
class ObjectsContext;
class IByteWriter;
class PDFDictionary;
class PDFArray;
class PDFStreamInput;
class DictionaryContext;
class PageContentContext;
class PDFPage;
class IDocumentContextExtender;
class IPageEmbedInFormCommand;

using namespace std;

typedef map<ObjectIDType,ObjectIDType> ObjectIDTypeToObjectIDTypeMap;
typedef map<string,string> StringToStringMap;
typedef set<ObjectIDType> ObjectIDTypeSet;
typedef set<IDocumentContextExtender*> IDocumentContextExtenderSet;

struct ResourceTokenMarker
{
	ResourceTokenMarker(string inResourceToken,LongFilePositionType inResourceTokenPosition)
	{
		ResourceToken = inResourceToken;
		ResourceTokenPosition = inResourceTokenPosition;
	}

	string ResourceToken;
	LongFilePositionType ResourceTokenPosition;
};

typedef list<ResourceTokenMarker> ResourceTokenMarkerList;

class PDFDocumentHandler : public DocumentContextExtenderAdapter
{
public:
	PDFDocumentHandler(void);
	virtual ~PDFDocumentHandler(void);

	void SetOperationsContexts(DocumentContext* inDocumentcontext,ObjectsContext* inObjectsContext);

	// Create a list of XObjects from a PDF file.
	// the list of objects can then be used to place the "pages" in various locations on the written
	// PDF page.
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);

	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const PDFRectangle& inCropBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);
	
	// appends pages from source PDF to the written PDF. returns object ID for the created pages
	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects);

	// MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content, cheaper than creating
	// and XObject and later placing, when the intention is to use this graphic just once.
	EStatusCode MergePDFPagesToPage(PDFPage* inPage,
									const wstring& inPDFFilePath,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects);

	// Event listeners for CreateFormXObjectsFromPDF and AppendPDFPagesFromPDF
	void AddDocumentContextExtender(IDocumentContextExtender* inExtender);
	void RemoveDocumentContextExtender(IDocumentContextExtender* inExtender);	

	// IDocumentContextExtender implementation
	virtual EStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext);


	// copying context handling
	EStatusCode StartFileCopyingContext(const wstring& inPDFFilePath);
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 EPDFPageBox inPageBoxToUseAsFormBox,
														 const double* inTransformationMatrix);
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 const PDFRectangle& inCropBox,
														 const double* inTransformationMatrix);
	EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	EStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);
	PDFParser* GetSourceDocumentParser();
	EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();
	void StopFileCopyingContext();


	// Internal implementation. do not use directly
	PDFFormXObject* CreatePDFFormXObjectForPage(unsigned long inPageIndex,
												EPDFPageBox inPageBoxToUseAsFormBox,
												const double* inTransformationMatrix);
	PDFFormXObject* CreatePDFFormXObjectForPage(unsigned long inPageIndex,
												const PDFRectangle& inCropBox,
												const double* inTransformationMatrix);
private:

	ObjectsContext* mObjectsContext;
	DocumentContext* mDocumentContext;
	IDocumentContextExtenderSet mExtenders;


	InputFile mPDFFile;
	PDFParser mParser;
	ObjectIDTypeToObjectIDTypeMap mSourceToTarget;
	PDFDictionary* mWrittenPage;


	PDFRectangle DeterminePageBox(PDFDictionary* inDictionary,EPDFPageBox inPageBoxType);
	void SetPDFRectangleFromPDFArray(PDFArray* inPDFArray,PDFRectangle& outPDFRectangle);
	double GetAsDoubleValue(PDFObject* inNumberObject);
	EStatusCode WritePageContentToSingleStream(IByteWriter* inTargetStream,PDFDictionary* inPageObject);
	EStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream);
	EStatusCode CopyResourcesIndirectObjects(PDFDictionary* inPage);
	void RegisterInDirectObjects(PDFDictionary* inDictionary,ObjectIDTypeList& outNewObjects);
	void RegisterInDirectObjects(PDFArray* inArray,ObjectIDTypeList& outNewObjects);
	EStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs);
	EStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs,ObjectIDTypeSet& ioCopiedObjects);
	EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID,ObjectIDTypeSet& ioCopiedObjects);
	EStatusCode WriteObjectByType(PDFObject* inObject,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCode WriteArrayObject(PDFArray* inArray,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
															const PDFPageRange& inPageRange,
															IPageEmbedInFormCommand* inPageEmbedCommand,
															const double* inTransformationMatrix,
															const ObjectIDTypeList& inCopyAdditionalObjects);
	PDFFormXObject* CreatePDFFormXObjectForPage(PDFDictionary* inPageObject,
												const PDFRectangle& inCropBox,
												const double* inTransformationMatrix);
	EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID);


	EStatusCode WriteDictionaryObject(PDFDictionary* inDictionary,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCode WriteObjectByType(PDFObject* inObject,DictionaryContext* inDictionaryContext,ObjectIDTypeList& outSourceObjectsToAdd);


	EStatusCode WriteStreamObject(PDFStreamInput* inStream,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCodeAndObjectIDType CreatePDFPageForPage(unsigned long inPageIndex);
	EStatusCode CopyPageContentToTargetPage(PDFPage* inPage,PDFDictionary* inPageObject);
	EStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource);
	PDFObject* QueryInheritedValue(PDFDictionary* inDictionary,string inName);
	EStatusCode MergePDFPageForPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	EStatusCode MergeResourcesToPage(PDFPage* inTargetPage,PDFDictionary* inPage,StringToStringMap& outMappedResourcesNames);
	EStatusCodeAndObjectIDType CopyObjectToIndirectObject(PDFObject* inObject);
	EStatusCode CopyDirectObjectToIndirectObject(PDFObject* inObject,ObjectIDType inTargetObjectID);
	EStatusCode MergePageContentToTargetPage(PDFPage* inTargetPage,PDFDictionary* inSourcePage,const StringToStringMap& inMappedResourcesNames);
	EStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource,const StringToStringMap& inMappedResourcesNames);
	EStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames);
	EStatusCode ScanStreamForResourcesTokens(PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames,ResourceTokenMarkerList& outResourceMarkers);
	EStatusCode MergeAndReplaceResourcesTokens(	IByteWriter* inTargetStream,
												PDFStreamInput* inSourceStream,
												const StringToStringMap& inMappedResourcesNames,
												const ResourceTokenMarkerList& inResourceMarkers);

	string AsEncodedName(const string& inName);

};

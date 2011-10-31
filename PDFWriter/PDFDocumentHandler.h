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

namespace PDFHummus
{
	class DocumentContext;
}

using namespace PDFHummus;
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

	void SetOperationsContexts(DocumentContext* inDocumentContext,ObjectsContext* inObjectsContext);

	// Create a list of XObjects from a PDF file.
	// the list of objects can then be used to place the "pages" in various locations on the written
	// PDF page.
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const string& inPDFFilePath,
																const PDFPageRange& inPageRange,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);

	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix,
															 const ObjectIDTypeList& inCopyAdditionalObjects);

	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const string& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const PDFRectangle& inCropBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);

	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix,
															 const ObjectIDTypeList& inCopyAdditionalObjects);

	// appends pages from source PDF to the written PDF. returns object ID for the created pages
	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const string& inPDFFilePath,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects);

	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects);

	// MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content, cheaper than creating
	// and XObject and later placing, when the intention is to use this graphic just once.
	PDFHummus::EStatusCode MergePDFPagesToPage(PDFPage* inPage,
									const string& inPDFFilePath,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects);

	PDFHummus::EStatusCode MergePDFPagesToPage(PDFPage* inPage,
									IByteReaderWithPosition* inPDFStream,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects);

	// Event listeners for CreateFormXObjectsFromPDF and AppendPDFPagesFromPDF
	void AddDocumentContextExtender(IDocumentContextExtender* inExtender);
	void RemoveDocumentContextExtender(IDocumentContextExtender* inExtender);

	// IDocumentContextExtender implementation
	virtual PDFHummus::EStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext);


	// copying context handling
	PDFHummus::EStatusCode StartFileCopyingContext(const string& inPDFFilePath);
	PDFHummus::EStatusCode StartStreamCopyingContext(IByteReaderWithPosition* inPDFStream);
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 EPDFPageBox inPageBoxToUseAsFormBox,
														 const double* inTransformationMatrix);
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 const PDFRectangle& inCropBox,
														 const double* inTransformationMatrix);
	EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	PDFHummus::EStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);
	PDFParser* GetSourceDocumentParser();
	EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();
	EStatusCodeAndObjectIDTypeList CopyDirectObject(PDFObject* inObject);
	PDFHummus::EStatusCode CopyNewObjectsForDirectObject(const ObjectIDTypeList& inReferencedObjects);
	void StopCopyingContext();


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
	IByteReaderWithPosition* mPDFStream;
	PDFParser mParser;
	ObjectIDTypeToObjectIDTypeMap mSourceToTarget;
	PDFDictionary* mWrittenPage;


	PDFRectangle DeterminePageBox(PDFDictionary* inDictionary,EPDFPageBox inPageBoxType);
	void SetPDFRectangleFromPDFArray(PDFArray* inPDFArray,PDFRectangle& outPDFRectangle);
	double GetAsDoubleValue(PDFObject* inNumberObject);
	PDFHummus::EStatusCode WritePageContentToSingleStream(IByteWriter* inTargetStream,PDFDictionary* inPageObject);
	PDFHummus::EStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream);
	PDFHummus::EStatusCode CopyResourcesIndirectObjects(PDFDictionary* inPage);
	void RegisterInDirectObjects(PDFDictionary* inDictionary,ObjectIDTypeList& outNewObjects);
	void RegisterInDirectObjects(PDFArray* inArray,ObjectIDTypeList& outNewObjects);
	PDFHummus::EStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs);
	PDFHummus::EStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs,ObjectIDTypeSet& ioCopiedObjects);
	PDFHummus::EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID,ObjectIDTypeSet& ioCopiedObjects);
	PDFHummus::EStatusCode WriteObjectByType(PDFObject* inObject,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	PDFHummus::EStatusCode WriteArrayObject(PDFArray* inArray,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const string& inPDFFilePath,
															const PDFPageRange& inPageRange,
															IPageEmbedInFormCommand* inPageEmbedCommand,
															const double* inTransformationMatrix,
															const ObjectIDTypeList& inCopyAdditionalObjects);
	PDFFormXObject* CreatePDFFormXObjectForPage(PDFDictionary* inPageObject,
												const PDFRectangle& inCropBox,
												const double* inTransformationMatrix);
	PDFHummus::EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID);


	PDFHummus::EStatusCode WriteDictionaryObject(PDFDictionary* inDictionary,ObjectIDTypeList& outSourceObjectsToAdd);
	PDFHummus::EStatusCode WriteObjectByType(PDFObject* inObject,DictionaryContext* inDictionaryContext,ObjectIDTypeList& outSourceObjectsToAdd);


	PDFHummus::EStatusCode WriteStreamObject(PDFStreamInput* inStream,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCodeAndObjectIDType CreatePDFPageForPage(unsigned long inPageIndex);
	PDFHummus::EStatusCode CopyPageContentToTargetPage(PDFPage* inPage,PDFDictionary* inPageObject);
	PDFHummus::EStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource);
	PDFObject* QueryInheritedValue(PDFDictionary* inDictionary,string inName);
	PDFHummus::EStatusCode MergePDFPageForPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	PDFHummus::EStatusCode MergeResourcesToPage(PDFPage* inTargetPage,PDFDictionary* inPage,StringToStringMap& outMappedResourcesNames);
	EStatusCodeAndObjectIDType CopyObjectToIndirectObject(PDFObject* inObject);
	PDFHummus::EStatusCode CopyDirectObjectToIndirectObject(PDFObject* inObject,ObjectIDType inTargetObjectID);
	PDFHummus::EStatusCode MergePageContentToTargetPage(PDFPage* inTargetPage,PDFDictionary* inSourcePage,const StringToStringMap& inMappedResourcesNames);
	PDFHummus::EStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource,const StringToStringMap& inMappedResourcesNames);
	PDFHummus::EStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames);
	PDFHummus::EStatusCode ScanStreamForResourcesTokens(PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames,ResourceTokenMarkerList& outResourceMarkers);
	PDFHummus::EStatusCode MergeAndReplaceResourcesTokens(	IByteWriter* inTargetStream,
												PDFStreamInput* inSourceStream,
												const StringToStringMap& inMappedResourcesNames,
												const ResourceTokenMarkerList& inResourceMarkers);

	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDFInContext(
																		const PDFPageRange& inPageRange,
																		IPageEmbedInFormCommand* inPageEmbedCommand,
																		const double* inTransformationMatrix,
																		const ObjectIDTypeList& inCopyAdditionalObjects);
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															const PDFPageRange& inPageRange,
															IPageEmbedInFormCommand* inPageEmbedCommand,
															const double* inTransformationMatrix,
															const ObjectIDTypeList& inCopyAdditionalObjects);
	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDFInContext(const PDFPageRange& inPageRange,
																  const ObjectIDTypeList& inCopyAdditionalObjects);
	PDFHummus::EStatusCode MergePDFPagesToPageInContext(PDFPage* inPage,
											const PDFPageRange& inPageRange,
											const ObjectIDTypeList& inCopyAdditionalObjects);
	PDFHummus::EStatusCode StartCopyingContext(IByteReaderWithPosition* inPDFStream);


	string AsEncodedName(const string& inName);

};

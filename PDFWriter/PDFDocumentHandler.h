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
#include "HummusPDFParser.h"
#include "PDFRectangle.h"
#include "InputFile.h"
#include "ObjectsBasicTypes.h"
#include "ETokenSeparator.h"
#include "DocumentsContextExtenderAdapter.h"
#include "MapIterator.h"

#include <map>
#include <list>
#include <set>

class DocumentsContext;
class ObjectsContext;
class IByteWriter;
class PDFDictionary;
class PDFArray;
class PDFStreamInput;
class DictionaryContext;
class PageContentContext;
class PDFPage;
class IDocumentsContextExtender;
class IPageEmbedInFormCommand;

using namespace std;

typedef map<ObjectIDType,ObjectIDType> ObjectIDTypeToObjectIDTypeMap;
typedef map<string,string> StringToStringMap;
typedef set<ObjectIDType> ObjectIDTypeSet;
typedef set<IDocumentsContextExtender*> IDocumentsContextExtenderSet;

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

class PDFDocumentHandler : public DocumentsContextExtenderAdapter
{
public:
	PDFDocumentHandler(void);
	virtual ~PDFDocumentHandler(void);

	void SetOperationsContexts(DocumentsContext* inDocumentsContext,ObjectsContext* inObjectsContext);

	// Create a list of XObjects from a PDF file.
	// the list of objects can then be used to place the "pages" in various locations on the written
	// PDF page.
	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix,
															 const ObjectIDTypeList& inCopyAdditionalObjects);

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const PDFRectangle& inCropBox,
																const double* inTransformationMatrix,
																const ObjectIDTypeList& inCopyAdditionalObjects);

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix,
															 const ObjectIDTypeList& inCopyAdditionalObjects);
	
	// appends pages from source PDF to the written PDF. returns object ID for the created pages
	EPDFStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects);

	EPDFStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects);

	// MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content, cheaper than creating
	// and XObject and later placing, when the intention is to use this graphic just once.
	EPDFStatusCode MergePDFPagesToPage(PDFPage* inPage,
									const wstring& inPDFFilePath,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects);

	EPDFStatusCode MergePDFPagesToPage(PDFPage* inPage,
									IByteReaderWithPosition* inPDFStream,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects);

	// Event listeners for CreateFormXObjectsFromPDF and AppendPDFPagesFromPDF
	void AddDocumentsContextExtender(IDocumentsContextExtender* inExtender);
	void RemoveDocumentsContextExtender(IDocumentsContextExtender* inExtender);	

	// IDocumentsContextExtender implementation
	virtual EPDFStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext);


	// copying context handling
	EPDFStatusCode StartFileCopyingContext(const wstring& inPDFFilePath);
	EPDFStatusCode StartStreamCopyingContext(IByteReaderWithPosition* inPDFStream);
	EPDFStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 EPDFPageBox inPageBoxToUseAsFormBox,
														 const double* inTransformationMatrix);
	EPDFStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
														 const PDFRectangle& inCropBox,
														 const double* inTransformationMatrix);
	EPDFStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	EPDFStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	EPDFStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);
	HummusPDFParser* GetSourceDocumentParser();
	EPDFStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();
	EPDFStatusCodeAndObjectIDTypeList CopyDirectObject(PDFObject* inObject);
	EPDFStatusCode CopyNewObjectsForDirectObject(const ObjectIDTypeList& inReferencedObjects);
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
	DocumentsContext* mDocumentsContext;
	IDocumentsContextExtenderSet mExtenders;


	InputFile mPDFFile;
	IByteReaderWithPosition* mPDFStream;
	HummusPDFParser mParser;
	ObjectIDTypeToObjectIDTypeMap mSourceToTarget;
	PDFDictionary* mWrittenPage;


	PDFRectangle DeterminePageBox(PDFDictionary* inDictionary,EPDFPageBox inPageBoxType);
	void SetPDFRectangleFromPDFArray(PDFArray* inPDFArray,PDFRectangle& outPDFRectangle);
	double GetAsDoubleValue(PDFObject* inNumberObject);
	EPDFStatusCode WritePageContentToSingleStream(IByteWriter* inTargetStream,PDFDictionary* inPageObject);
	EPDFStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream);
	EPDFStatusCode CopyResourcesIndirectObjects(PDFDictionary* inPage);
	void RegisterInDirectObjects(PDFDictionary* inDictionary,ObjectIDTypeList& outNewObjects);
	void RegisterInDirectObjects(PDFArray* inArray,ObjectIDTypeList& outNewObjects);
	EPDFStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs);
	EPDFStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs,ObjectIDTypeSet& ioCopiedObjects);
	EPDFStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID,ObjectIDTypeSet& ioCopiedObjects);
	EPDFStatusCode WriteObjectByType(PDFObject* inObject,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EPDFStatusCode WriteArrayObject(PDFArray* inArray,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
															const PDFPageRange& inPageRange,
															IPageEmbedInFormCommand* inPageEmbedCommand,
															const double* inTransformationMatrix,
															const ObjectIDTypeList& inCopyAdditionalObjects);
	PDFFormXObject* CreatePDFFormXObjectForPage(PDFDictionary* inPageObject,
												const PDFRectangle& inCropBox,
												const double* inTransformationMatrix);
	EPDFStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID);


	EPDFStatusCode WriteDictionaryObject(PDFDictionary* inDictionary,ObjectIDTypeList& outSourceObjectsToAdd);
	EPDFStatusCode WriteObjectByType(PDFObject* inObject,DictionaryContext* inDictionaryContext,ObjectIDTypeList& outSourceObjectsToAdd);


	EPDFStatusCode WriteStreamObject(PDFStreamInput* inStream,ObjectIDTypeList& outSourceObjectsToAdd);
	EPDFStatusCodeAndObjectIDType CreatePDFPageForPage(unsigned long inPageIndex);
	EPDFStatusCode CopyPageContentToTargetPage(PDFPage* inPage,PDFDictionary* inPageObject);
	EPDFStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource);
	PDFObject* QueryInheritedValue(PDFDictionary* inDictionary,string inName);
	EPDFStatusCode MergePDFPageForPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	EPDFStatusCode MergeResourcesToPage(PDFPage* inTargetPage,PDFDictionary* inPage,StringToStringMap& outMappedResourcesNames);
	EPDFStatusCodeAndObjectIDType CopyObjectToIndirectObject(PDFObject* inObject);
	EPDFStatusCode CopyDirectObjectToIndirectObject(PDFObject* inObject,ObjectIDType inTargetObjectID);
	EPDFStatusCode MergePageContentToTargetPage(PDFPage* inTargetPage,PDFDictionary* inSourcePage,const StringToStringMap& inMappedResourcesNames);
	EPDFStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource,const StringToStringMap& inMappedResourcesNames);
	EPDFStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames);
	EPDFStatusCode ScanStreamForResourcesTokens(PDFStreamInput* inSourceStream,const StringToStringMap& inMappedResourcesNames,ResourceTokenMarkerList& outResourceMarkers);
	EPDFStatusCode MergeAndReplaceResourcesTokens(	IByteWriter* inTargetStream,
												PDFStreamInput* inSourceStream,
												const StringToStringMap& inMappedResourcesNames,
												const ResourceTokenMarkerList& inResourceMarkers);

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDFInContext(
																		const PDFPageRange& inPageRange,
																		IPageEmbedInFormCommand* inPageEmbedCommand,
																		const double* inTransformationMatrix,
																		const ObjectIDTypeList& inCopyAdditionalObjects);
	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															const PDFPageRange& inPageRange,
															IPageEmbedInFormCommand* inPageEmbedCommand,
															const double* inTransformationMatrix,
															const ObjectIDTypeList& inCopyAdditionalObjects);
	EPDFStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDFInContext(const PDFPageRange& inPageRange,
																  const ObjectIDTypeList& inCopyAdditionalObjects);
	EPDFStatusCode MergePDFPagesToPageInContext(PDFPage* inPage,
											const PDFPageRange& inPageRange,
											const ObjectIDTypeList& inCopyAdditionalObjects);
	EPDFStatusCode StartCopyingContext(IByteReaderWithPosition* inPDFStream);


	string AsEncodedName(const string& inName);

};

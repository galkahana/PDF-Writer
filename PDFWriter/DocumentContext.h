/*
   Source File : DocumentContext.h


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

#include "EStatusCode.h"
#include "EPDFVersion.h"
#include "IOBasicTypes.h"
#include "TrailerInformation.h"
#include "CatalogInformation.h"
#include "JPEGImageHandler.h"
#include "TIFFImageHandler.h"
#include "TIFFUsageParameters.h"
#include "UsedFontsRepository.h"
#include "PDFEmbedParameterTypes.h"
#include "PDFDocumentHandler.h"
#include "ObjectsBasicTypes.h"

#include <string>
#include <set>
#include <utility>
#include <list>
#include <map>

using namespace std;
using namespace IOBasicTypes;

class DictionaryContext;
class ObjectsContext;
class PDFPage;
class PageTree;
class OutputFile;
class IDocumentContextExtender;
class PageContentContext;
class ResourcesDictionary;
class PDFFormXObject;
class PDFRectangle;
class PDFImageXObject;
class PDFUsedFont;
class PageContentContext;
class PDFParser;
class PDFDictionary;
class PDFDocumentCopyingContext;

typedef set<IDocumentContextExtender*> IDocumentContextExtenderSet;
typedef pair<PDFHummus::EStatusCode,ObjectIDType> EStatusCodeAndObjectIDType;
typedef list<ObjectIDType> ObjectIDTypeList;
typedef map<ObjectIDType,string> ObjectIDTypeToStringMap;
typedef set<PDFDocumentCopyingContext*> PDFDocumentCopyingContextSet;

namespace PDFHummus
{
	class DocumentContext
	{
	public:
		DocumentContext();
		~DocumentContext();

		void SetObjectsContext(ObjectsContext* inObjectsContext);
		void SetOutputFileInformation(OutputFile* inOutputFile);
		PDFHummus::EStatusCode	WriteHeader(EPDFVersion inPDFVersion);
		PDFHummus::EStatusCode	FinalizePDF();

		TrailerInformation& GetTrailerInformation();
		CatalogInformation& GetCatalogInformation();

		// Page and Page Content Writing

		// Use StartPageContentContext for creating a page content for this page.
		// using the context you can now start creating content for it.
		// if this page already has a content context, it will be returned
		PageContentContext* StartPageContentContext(PDFPage* inPage);

		// Use PausePageContentContext in order to flush a current content write, in order
		// to write some other object. A common use case is when wanting to use an image in a page.
		// first the content before the image is written, then the content is paused, a new object that represents the image
		// is written, and then the content continues showing the image with a "do" operator. This is also the cause for creating multiple content
		// streams for a page (and what will happen in this implementation as well).
		PDFHummus::EStatusCode PausePageContentContext(PageContentContext* inPageContext);

		// Finalize and release the page context. the current content stream is flushed to the PDF stream
		PDFHummus::EStatusCode EndPageContentContext(PageContentContext* inPageContext);

		// Determine whether this page already has a content context
		bool HasContentContext(PDFPage* inPage);
		
		EStatusCodeAndObjectIDType WritePage(PDFPage* inPage);
		EStatusCodeAndObjectIDType WritePageAndRelease(PDFPage* inPage);

		// Use this to add annotation references to a page. the references will be written on the next page write (see WritePage and WritePageAndRelease)
		void RegisterAnnotationReferenceForNextPageWrite(ObjectIDType inAnnotationReference);

		// Form XObject creation and finalization
		PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix = NULL);
		PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix = NULL);
		PDFHummus::EStatusCode EndFormXObjectAndRelease(PDFFormXObject* inFormXObject);

		// no release version of ending a form XObject. owner should delete it (regular delete...nothin special)
		PDFHummus::EStatusCode EndFormXObjectNoRelease(PDFFormXObject* inFormXObject);

		// Image XObject creating. 
		// note that as oppose to other methods, create the image xobject also writes it, so there's no "WriteXXXXAndRelease" for image.
		// So...release the object yourself [just delete it]

		// JPEG - two variants
		
		// will return image xobject sized at 1X1
		PDFImageXObject* CreateImageXObjectFromJPGFile(const string& inJPGFilePath);
		PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
		PDFImageXObject* CreateImageXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inImageXObjectID);
		PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID);

		// will return form XObject, which will include the xobject at it's size
		PDFFormXObject* CreateFormXObjectFromJPGFile(const string& inJPGFilePath);
		PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
		PDFFormXObject* CreateFormXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inFormXObjectID);
		PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID);

		// TIFF
		PDFFormXObject* CreateFormXObjectFromTIFFFile(	const string& inTIFFFilePath,
														const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
		PDFFormXObject* CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
														const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
		PDFFormXObject* CreateFormXObjectFromTIFFFile(	const string& inTIFFFilePath,
														ObjectIDType inFormXObjectID,
														const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
		PDFFormXObject* CreateFormXObjectFromTIFFStream(	IByteReaderWithPosition* inTIFFStream,
														ObjectIDType inFormXObjectID,
														const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
		
		// PDF
		// CreateFormXObjectsFromPDF is for using input PDF pages as objects in one page or more. you can used the returned IDs to place the 
		// created form xobjects
		EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const string& inPDFFilePath,
																 const PDFPageRange& inPageRange,
																 EPDFPageBox inPageBoxToUseAsFormBox,
																 const double* inTransformationMatrix = NULL,
																 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																 const PDFPageRange& inPageRange,
																 EPDFPageBox inPageBoxToUseAsFormBox,
																 const double* inTransformationMatrix = NULL,
																 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());
		
		// CreateFormXObjectsFromPDF is an override to allow you to determine a custom crop for the page embed
		EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const string& inPDFFilePath,
																 const PDFPageRange& inPageRange,
																 const PDFRectangle& inCropBox,
																 const double* inTransformationMatrix = NULL,
																 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																 const PDFPageRange& inPageRange,
																 const PDFRectangle& inCropBox,
																 const double* inTransformationMatrix = NULL,
																 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		// AppendPDFPagesFromPDF is for simple appending of the input PDF pages
		EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const string& inPDFFilePath,
															const PDFPageRange& inPageRange,
															const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());
		
		EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
															const PDFPageRange& inPageRange,
															const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		// MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content, cheaper than creating
		// and XObject and later placing, when the intention is to use this graphic just once.
		PDFHummus::EStatusCode MergePDFPagesToPage(PDFPage* inPage,
										const string& inPDFFilePath,
										const PDFPageRange& inPageRange,
										const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		PDFHummus::EStatusCode MergePDFPagesToPage(PDFPage* inPage,
										IByteReaderWithPosition* inPDFStream,
										const PDFPageRange& inPageRange,
										const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

		PDFDocumentCopyingContext* CreatePDFCopyingContext(const string& inPDFFilePath);

		PDFDocumentCopyingContext* CreatePDFCopyingContext(IByteReaderWithPosition* inPDFStream);

		// Font [Text]
		PDFUsedFont* GetFontForFile(const string& inFontFilePath);
		// second overload is for type 1, when an additional metrics file is available
		PDFUsedFont* GetFontForFile(const string& inFontFilePath,const string& inAdditionalMeticsFilePath);

		// URL should be encoded to be a valid URL, ain't gonna be checking that!
		PDFHummus::EStatusCode AttachURLLinktoCurrentPage(const string& inURL,const PDFRectangle& inLinkClickArea);

		// Extensibility
		void AddDocumentContextExtender(IDocumentContextExtender* inExtender);
		void RemoveDocumentContextExtender(IDocumentContextExtender* inExtender);
		void SetParserExtender(IPDFParserExtender* inParserExtender);

		// JPG images handler for retrieving JPG images information
		JPEGImageHandler& GetJPEGImageHandler();


		PDFHummus::EStatusCode WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
		PDFHummus::EStatusCode ReadState(PDFParser* inStateReader,ObjectIDType inObjectID);

		void Cleanup();
		
		// internal methods for copying context listeners handling
		void RegisterCopyingContext(PDFDocumentCopyingContext* inCopyingContext);
		void UnRegisterCopyingContext(PDFDocumentCopyingContext* inCopyingContext);
	private:
		ObjectsContext* mObjectsContext;
		TrailerInformation mTrailerInformation;
		CatalogInformation mCatalogInformation;
		string mOutputFilePath;
		IDocumentContextExtenderSet mExtenders;
		JPEGImageHandler mJPEGImageHandler;
		TIFFImageHandler mTIFFImageHandler;
		PDFDocumentHandler mPDFDocumentHandler;
		UsedFontsRepository mUsedFontsRepository;
		ObjectIDTypeList mAnnotations;
		IPDFParserExtender* mParserExtender;
		PDFDocumentCopyingContextSet mCopyingContexts;
		
		void WriteHeaderComment(EPDFVersion inPDFVersion);
		void Write4BinaryBytes();
		PDFHummus::EStatusCode WriteCatalogObject();
		PDFHummus::EStatusCode WriteTrailerDictionary();
		void WriteXrefReference(LongFilePositionType inXrefTablePosition);
		void WriteFinalEOF();
		void WriteInfoDictionary();
		void WritePagesTree();
		int WritePageTree(PageTree* inPageTreeToWrite);
		string GenerateMD5IDForFile();
		PDFHummus::EStatusCode WriteResourcesDictionary(ResourcesDictionary& inResourcesDictionary);
		void WriteResourceDictionary(DictionaryContext* inResourcesDictionary,
									const string& inResourceDictionaryLabel,
									MapIterator<ObjectIDTypeToStringMap> inMapping);
		bool IsIdentityMatrix(const double* inMatrix);
		PDFHummus::EStatusCode WriteUsedFontsDefinitions();
		EStatusCodeAndObjectIDType WriteAnnotationAndLinkForURL(const string& inURL,const PDFRectangle& inLinkClickArea);

		void WriteTrailerState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
		void WriteTrailerInfoState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
		void WriteDateState(ObjectsContext* inStateWriter,const PDFDate& inDate);
		void WriteCatalogInformationState(ObjectsContext* inStateWriter,ObjectIDType inObjectID);
		void ReadTrailerState(PDFParser* inStateReader,PDFDictionary* inTrailerState);
		void ReadTrailerInfoState(PDFParser* inStateReader,PDFDictionary* inTrailerInfoState);
		void ReadDateState(PDFDictionary* inDateState,PDFDate& inDate);
		void ReadCatalogInformationState(PDFParser* inStateReader,PDFDictionary* inCatalogInformationState);

		ObjectIDType mCurrentPageTreeIDInState;

		void WritePageTreeState(ObjectsContext* inStateWriter,ObjectIDType inObjectID,PageTree* inPageTree);
		void ReadPageTreeState(PDFParser* inStateReader,PDFDictionary* inPageTreeState,PageTree* inPageTree);

	};
}
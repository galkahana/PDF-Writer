#pragma once

#include "PDFEmbedParameterTypes.h"
#include "PDFParser.h"
#include "PDFRectangle.h"
#include "InputFile.h"
#include "ObjectsBasicTypes.h"
#include "ETokenSeparator.h"
#include "DocumentContextExtenderAdapter.h"

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

using namespace std;

typedef map<ObjectIDType,ObjectIDType> ObjectIDTypeToObjectIDTypeMap;
typedef set<ObjectIDType> ObjectIDTypeSet;


class PDFDocumentHandler : public DocumentContextExtenderAdapter
{
public:
	PDFDocumentHandler(void);
	virtual ~PDFDocumentHandler(void);

	void SetOperationsContexts(DocumentContext* inDocumentcontext,ObjectsContext* inObjectsContext);

	// Create a list of XObjects from a PDF file.
	// the list of objects can then be used to place the "pages" in various locations on the written
	// PDF page.
	EStatusCodeAndPDFFormXObjectList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix);
	
	// appends pages from source PDF to the written PDF. returns object ID for the created pages
	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
														const PDFPageRange& inPageRange);


	// IDocumentContextExtender implementation
	virtual EStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext);

private:

	ObjectsContext* mObjectsContext;
	DocumentContext* mDocumentContext;

	InputFile mPDFFile;
	PDFParser mParser;
	ObjectIDTypeToObjectIDTypeMap mSourceToTarget;
	PDFDictionary* mWrittenPage;

	PDFFormXObject* CreatePDFFormXObjectForPage(unsigned long inPageIndex,
												EPDFPageBox inPageBoxToUseAsFormBox,
												const double* inTransformationMatrix);
	PDFRectangle DeterminePageBox(PDFDictionary* inDictionary,EPDFPageBox inPageBoxType);
	void SetPDFRectangleFromPDFArray(PDFArray* inPDFArray,PDFRectangle& outPDFRectangle);
	double GetAsDoubleValue(PDFObject* inNumberObject);
	EStatusCode WritePageContentToSingleStream(IByteWriter* inTargetStream,PDFDictionary* inPageObject);
	EStatusCode WritePDFStreamInputToStream(IByteWriter* inTargetStream,PDFStreamInput* inSourceStream);
	EStatusCode CopyResourcesIndirectObjects(PDFDictionary* inPage);
	void RegisterInDirectObjects(PDFDictionary* inDictionary,ObjectIDTypeList& outNewObjects);
	void RegisterInDirectObjects(PDFArray* inArray,ObjectIDTypeList& outNewObjects);
	EStatusCode WriteNewObjects(const ObjectIDTypeList& inSourceObjectIDs,ObjectIDTypeSet& ioCopiedObjects);
	EStatusCode CopyInDirectObject(ObjectIDType inSourceObjectID,ObjectIDType inTargetObjectID,ObjectIDTypeSet& ioCopiedObjects);
	EStatusCode WriteObjectByType(PDFObject* inObject,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCode WriteArrayObject(PDFArray* inArray,ETokenSeparator inSeparator,ObjectIDTypeList& outSourceObjectsToAdd);

	EStatusCode WriteDictionaryObject(PDFDictionary* inDictionary,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCode WriteObjectByType(PDFObject* inObject,DictionaryContext* inDictionaryContext,ObjectIDTypeList& outSourceObjectsToAdd);


	EStatusCode WriteStreamObject(PDFStreamInput* inStream,ObjectIDTypeList& outSourceObjectsToAdd);
	EStatusCodeAndObjectIDType CreatePDFPageForPage(unsigned long inPageIndex);
	EStatusCode CopyPageContentToTargetPage(PDFPage* inPage,PDFDictionary* inPageObject);
	EStatusCode WritePDFStreamInputToContentContext(PageContentContext* inContentContext,PDFStreamInput* inContentSource);


};

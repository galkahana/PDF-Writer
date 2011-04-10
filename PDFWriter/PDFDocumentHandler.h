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

using namespace std;

typedef map<ObjectIDType,ObjectIDType> ObjectIDTypeToObjectIDTypeMap;
typedef list<ObjectIDType> ObjectIDTypeList;
typedef set<ObjectIDType> ObjectIDTypeSet;


class PDFDocumentHandler : public DocumentContextExtenderAdapter
{
public:
	PDFDocumentHandler(void);
	virtual ~PDFDocumentHandler(void);

	void SetOperationsContexts(DocumentContext* inDocumentcontext,ObjectsContext* inObjectsContext);

	EStatusCodeAndPDFFormXObjectList CreateFormXObjectsFromPDF( const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																EPDFPageBox inPageBoxToUseAsFormBox,
																const double* inTransformationMatrix);

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


};

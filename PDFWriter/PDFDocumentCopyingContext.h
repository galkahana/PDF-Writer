#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "PDFEmbedParameterTypes.h"
#include "PDFDocumentHandler.h"

#include <string>

using namespace std;

class DocumentContext;
class ObjectsContext;
class PDFParser;


class PDFDocumentCopyingContext
{
public:
	PDFDocumentCopyingContext();
	~PDFDocumentCopyingContext(void);

	EStatusCode Start(const wstring& inPDFFilePath,
					  DocumentContext* inDocumentcontext,
					  ObjectsContext* inObjectsContext);

	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL);
	EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	
	EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);

	
	PDFParser* GetSourceDocumentParser();
	EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();

	void End();
private:

	PDFDocumentHandler mDocumentHandler;


};

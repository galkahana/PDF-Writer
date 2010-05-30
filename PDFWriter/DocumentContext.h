#pragma once

#include "EStatusCode.h"
#include "EPDFVersion.h"
#include "IOBasicTypes.h"
#include "TrailerInformation.h"
#include "CatalogInformation.h"

#include <string>

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

class DocumentContext
{
public:
	DocumentContext(void);
	~DocumentContext(void);

	void SetObjectsContext(ObjectsContext* inObjectsContext);
	void SetOutputFileInformation(OutputFile* inOutputFile);
	EStatusCode	WriteHeader(EPDFVersion inPDFVersion);
	EStatusCode	FinalizePDF();

	TrailerInformation& GetTrailerInformation();
	CatalogInformation& GetCatalogInformation();

	// Page and Page Content Writing

	// Use StartPageContentContext for creating a page content for this page.
	// using the context you can now start creating content for it.
	PageContentContext* StartPageContentContext(PDFPage* inPage);

	// Use PausePageContentContext in order to flush a current content write, in order
	// to write some other object. A common use case is when wanting to use an image in a page.
	// first the content before the image is written, then the content is paused, a new object that represents the image
	// is written, and then the content continues showing the image with a "do" operator. This is also the cause for creating multiple content
	// streams for a page (and what will happen in this implementation as well).
	EStatusCode PausePageContentContext(PageContentContext* inPageContext);

	// Finalize and release the page context. the current content stream is flushed to the PDF stream
	EStatusCode EndPageContentContext(PageContentContext* inPageContext);
	
	EStatusCode WritePage(PDFPage* inPage);
	EStatusCode WritePageAndRelease(PDFPage* inPage);


	// Form XObject creation and writing
	PDFFormXObject* CreateFormXObject();
	EStatusCode WriteFormXObjectAndRelease(PDFFormXObject* inFormXObject);

	// Extensibility
	void SetDocumentContextExtender(IDocumentContextExtender* inExtender);

private:
	ObjectsContext* mObjectsContext;
	TrailerInformation mTrailerInformation;
	CatalogInformation mCatalogInformation;
	wstring mOutputFilePath;
	IDocumentContextExtender* mExtender;
	
	void WriteHeaderComment(EPDFVersion inPDFVersion);
	void Write4BinaryBytes();
	EStatusCode WriteCatalogObject();
	EStatusCode WriteTrailerDictionary();
	void WriteXrefReference(LongFilePositionType inXrefTablePosition);
	void WriteFinalEOF();
	void WriteInfoDictionary();
	void WritePagesTree();
	int WritePageTree(PageTree* inPageTreeToWrite);
	string GenerateMD5IDForFile();
	EStatusCode WriteResourcesDictionary(ResourcesDictionary& inResourcesDictionary,DictionaryContext* inParentDictionaryContext);
	bool IsIdentityMatrix(const double* inMatrix);
};

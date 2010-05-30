#pragma once

class PDFPage;
class DictionaryContext;
class ObjectsContext;
class DocumentContext;
class CatalogInformation;
class ResourcesDictionary;
class PDFFormXObject;

class IDocumentContextExtender
{
public:
	// add items to the page dictionary while it's written
	virtual EStatusCode OnPageWrite(
							PDFPage* inPage,
							DictionaryContext* inPageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;

	// add items to the resources dictionary while it's written (can be either page or xobject resources dictionary)
	virtual EStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;

	// add items to the form dictionary while it's written
	virtual EStatusCode OnFormXObjectWrite(
							PDFFormXObject* inFormXObject,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;

	// add items to catalog dictionary while it's written
	virtual EStatusCode OnCatalogWrite(
							CatalogInformation* inCatalogInformation,
							DictionaryContext* inCatalogDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;
};
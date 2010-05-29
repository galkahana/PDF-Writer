#pragma once

class PDFPage;
class DictionaryContext;
class ObjectsContext;
class DocumentContext;
class CatalogInformation;

class IDocumentContextExtender
{
public:
	// add items to the page dictionary while it's written
	virtual EStatusCode OnPageWrite(
							PDFPage* inPage,
							DictionaryContext* inPageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;

	// add items to the page resources dictionary while it's written
	virtual EStatusCode OnPageResourcesWrite(
							PDFPage* inPage,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext) = 0;

	// add items to the procset array in the page resource dictionary while it's written
	virtual EStatusCode OnPageResourceProcsetsWrite(
							PDFPage* inPage,
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
#include "IDocumentContextExtender.h"

// adapter class for IDocumentContextExtender interface.
// note that you can't create it, just derive from it.

class DocumentContextExtenderAdapter: public IDocumentContextExtender
{
public:

	// add items to the page dictionary while it's written
	virtual EStatusCode OnPageWrite(
							PDFPage* inPage,
							DictionaryContext* inPageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext){return eSuccess;}

	// add items to the resources dictionary while it's written (can be either page or xobject resources dictionary)
	virtual EStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext){return eSuccess;}

	// add items to the form dictionary while it's written
	virtual EStatusCode OnFormXObjectWrite(
							ObjectIDType inFormXObjectID,
							ObjectIDType inFormXObjectResourcesDictionaryID,
							DictionaryContext* inFormDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext){return eSuccess;}

	// add items to the image dictionary while it's written
	virtual EStatusCode OnJPEGImageXObjectWrite(
							ObjectIDType inImageXObjectID,
							DictionaryContext* inImageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							JPEGImageHandler* inJPGImageHandler){return eSuccess;}

	// add items to the image dictionary while it's writtern for a TIFF image (for tile images there are multiple such images)
	virtual EStatusCode OnTIFFImageXObjectWrite(
							ObjectIDType inImageXObjectID,
							DictionaryContext* inImageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							TIFFImageHandler* inTIFFImageHandler){return eSuccess;}


	// add items to catalog dictionary while it's written
	virtual EStatusCode OnCatalogWrite(
							CatalogInformation* inCatalogInformation,
							DictionaryContext* inCatalogDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext){return eSuccess;}

protected:
	DocumentContextExtenderAdapter(){}

}
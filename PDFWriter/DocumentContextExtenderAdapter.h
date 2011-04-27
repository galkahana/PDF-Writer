/*
   Source File : DocumentContextExtenderAdapter.h


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

	// pdf copying events

	virtual EStatusCode OnPDFParsingComplete(
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

	virtual EStatusCode OnBeforeCreateXObjectFromPage(
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

	virtual EStatusCode OnBeforeCreatePageFromPage(
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

	virtual EStatusCode OnAfterCreateXObjectFromPage(
							PDFFormXObject* iPageObjectResultXObject,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

	virtual EStatusCode OnAfterCreatePageFromPage(
							PDFPage* iPageObjectResultPage,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

	virtual EStatusCode OnPDFCopyingComplete(
							ObjectsContext* inPDFWriterObjectContext,
							DocumentContext* inPDFWriterDocumentContext,
							PDFDocumentHandler* inPDFDocumentHandler){return eSuccess;}

protected:
	DocumentContextExtenderAdapter(){}

};
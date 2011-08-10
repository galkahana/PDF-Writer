/*
   Source File : IDocumentsContextExtender.h


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

#include "EPDFStatusCode.h"

class PDFPage;
class DictionaryContext;
class ObjectsContext;
class DocumentsContext;
class CatalogInformation;
class ResourcesDictionary;
class PDFFormXObject;
class JPEGImageHandler;
class TIFFImageHandler;
class PDFDocumentHandler;
class PDFDictionary;

class IDocumentsContextExtender
{
public:
	virtual ~IDocumentsContextExtender(){}

	// add items to the page dictionary while it's written
	virtual EPDFStatusCode OnPageWrite(
							PDFPage* inPage,
							DictionaryContext* inPageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext) = 0;

	// add items to the resources dictionary while it's written (can be either page or xobject resources dictionary)
	virtual EPDFStatusCode OnResourcesWrite(
							ResourcesDictionary* inResources,
							DictionaryContext* inPageResourcesDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext) = 0;

	// add items to a particular resource dictionary (will be called from all but procset array and xobjects dict)
	virtual EPDFStatusCode OnResourceDictionaryWrite(
							DictionaryContext* inResourceDictionary,
							const string& inResourceDictionaryName,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext) = 0;

	// add items to the form dictionary while it's written
	virtual EPDFStatusCode OnFormXObjectWrite(
							ObjectIDType inFormXObjectID,
							ObjectIDType inFormXObjectResourcesDictionaryID,
							DictionaryContext* inFormDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext) = 0;

	// add items to the image dictionary while it's written for a JPG Image
	virtual EPDFStatusCode OnJPEGImageXObjectWrite(
							ObjectIDType inImageXObjectID,
							DictionaryContext* inImageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							JPEGImageHandler* inJPGImageHandler) = 0;

	// add items to the image dictionary while it's writtern for a TIFF image (for tile images there are multiple such images)
	virtual EPDFStatusCode OnTIFFImageXObjectWrite(
							ObjectIDType inImageXObjectID,
							DictionaryContext* inImageDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							TIFFImageHandler* inTIFFImageHandler) = 0;

	// add items to catalog dictionary while it's written
	virtual EPDFStatusCode OnCatalogWrite(
							CatalogInformation* inCatalogInformation,
							DictionaryContext* inCatalogDictionaryContext,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext) = 0;

	// PDF document embedding events

	// When using any embedding method - Parsing of PDF to merge is not complete, before starting any merging
	virtual EPDFStatusCode OnPDFParsingComplete(
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When creating XObjects from pages - before creating a particular page xobject
	virtual EPDFStatusCode OnBeforeCreateXObjectFromPage(
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When creating XObjects from pages - after creating a particular page xobject
	virtual EPDFStatusCode OnAfterCreateXObjectFromPage(
							PDFFormXObject* iPageObjectResultXObject,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When appending pages from PDF - before appending a particular page
	virtual EPDFStatusCode OnBeforeCreatePageFromPage(
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When appending pages from PDF - after appending a particular page
	virtual EPDFStatusCode OnAfterCreatePageFromPage(
							PDFPage* iPageObjectResultPage,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When merging pages from PDF - before merging a particular page
	virtual EPDFStatusCode OnBeforeMergePageFromPage(
							PDFPage* inTargetPage,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

	// When merging pages from PDF - after merging a particular page
	virtual EPDFStatusCode OnAfterMergePageFromPage(
							PDFPage* inTargetPage,
							PDFDictionary* inPageObjectDictionary,
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;


	// When using any embedding method - right after embedding of the PDF is complete
	virtual EPDFStatusCode OnPDFCopyingComplete(
							ObjectsContext* inPDFWriterObjectContext,
							DocumentsContext* inPDFWriterDocumentsContext,
							PDFDocumentHandler* inPDFDocumentHandler) = 0;

};
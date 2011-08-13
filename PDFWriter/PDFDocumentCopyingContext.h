/*
   Source File : PDFDocumentCopyingContext.h


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
#include "ObjectsBasicTypes.h"
#include "PDFEmbedParameterTypes.h"
#include "PDFDocumentHandler.h"

#include <string>

using namespace std;

class DocumentsContext;
class ObjectsContext;
class HummusPDFParser;


class PDFDocumentCopyingContext
{
public:
	PDFDocumentCopyingContext();
	~PDFDocumentCopyingContext(void);

	EPDFStatusCode Start(const string& inPDFFilePath,
					  DocumentsContext* inDocumentsContext,
					  ObjectsContext* inObjectsContext);

	EPDFStatusCode Start(IByteReaderWithPosition* inPDFStream,
					  DocumentsContext* inDocumentsContext,
					  ObjectsContext* inObjectsContext);

	EPDFStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL);
	EPDFStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix = NULL);
	EPDFStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	EPDFStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	
	EPDFStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);

	/* This one is a bit tricky:
		It copies a direct object, and returns the objects that it references, and still require copying. 
		YOU ARE REQUIRED TO COPY THEM IMMEDIATELY LATER using CopyNewObjectsForDirectObject...otherwise the PDF will be fault.
		The reason for not writing them at point of writing the direct object is simple - 100% of your usages of this
		method it will be in the context of writing something encapsulating this code...you don't want to be disturbed.
		still...there might be referenced objects from this object to copy, hence you must later copy them. The internal
		state of the copying context will hold object references for them till you do, so that the reference IDs remain good.
	*/
	EPDFStatusCodeAndObjectIDTypeList CopyDirectObject(PDFObject* inObject);
	/*
		Call this ONLY with the result of CopyDirectObject, to copy new objects that are reuqired for direct object. It is OK to merge a couple of results from multiple
		CopyDirectObject to a single list. MAKE SURE THERE ARE NO DUPLICATES in that case.
	*/
	EPDFStatusCode CopyNewObjectsForDirectObject(const ObjectIDTypeList& inReferencedObjects);

	HummusPDFParser* GetSourceDocumentParser();
	EPDFStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();

	void End();
private:

	PDFDocumentHandler mDocumentHandler;


};

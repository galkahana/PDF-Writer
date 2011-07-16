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
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix = NULL);
	EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	EStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	
	EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);

	
	PDFParser* GetSourceDocumentParser();
	EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();

	void End();
private:

	PDFDocumentHandler mDocumentHandler;


};

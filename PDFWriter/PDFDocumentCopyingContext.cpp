/*
   Source File : PDFDocumentCopyingContext.cpp


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
#include "PDFDocumentCopyingContext.h"

PDFDocumentCopyingContext::PDFDocumentCopyingContext(void)
{
}

PDFDocumentCopyingContext::~PDFDocumentCopyingContext(void)
{
	End();
}

EStatusCode PDFDocumentCopyingContext::Start(const wstring& inPDFFilePath,
											  DocumentContext* inDocumentContext,
											  ObjectsContext* inObjectsContext)
{
	mDocumentHandler.SetOperationsContexts(inDocumentContext,inObjectsContext);
	return mDocumentHandler.StartFileCopyingContext(inPDFFilePath);
}

EStatusCodeAndObjectIDType PDFDocumentCopyingContext::CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
																					EPDFPageBox inPageBoxToUseAsFormBox,
																					const double* inTransformationMatrix)
{
	return mDocumentHandler.CreateFormXObjectFromPDFPage(inPageIndex,inPageBoxToUseAsFormBox,inTransformationMatrix);
}

EStatusCodeAndObjectIDType PDFDocumentCopyingContext::AppendPDFPageFromPDF(unsigned long inPageIndex)
{
	return mDocumentHandler.AppendPDFPageFromPDF(inPageIndex);
}

EStatusCodeAndObjectIDType PDFDocumentCopyingContext::CopyObject(ObjectIDType inSourceObjectID)
{
	return mDocumentHandler.CopyObject(inSourceObjectID);
}

EStatusCodeAndObjectIDType PDFDocumentCopyingContext::GetCopiedObjectID(ObjectIDType inSourceObjectID)
{
	return mDocumentHandler.GetCopiedObjectID(inSourceObjectID);
}

PDFParser* PDFDocumentCopyingContext::GetSourceDocumentParser()
{
	return mDocumentHandler.GetSourceDocumentParser();
}

MapIterator<ObjectIDTypeToObjectIDTypeMap> PDFDocumentCopyingContext::GetCopiedObjectsMappingIterator()
{
	return mDocumentHandler.GetCopiedObjectsMappingIterator();
}


void PDFDocumentCopyingContext::End()
{
	return mDocumentHandler.StopFileCopyingContext();
}

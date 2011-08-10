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

EPDFStatusCode PDFDocumentCopyingContext::Start(const wstring& inPDFFilePath,
											  DocumentsContext* inDocumentsContext,
											  ObjectsContext* inObjectsContext)
{
	mDocumentHandler.SetOperationsContexts(inDocumentsContext,inObjectsContext);
	return mDocumentHandler.StartFileCopyingContext(inPDFFilePath);
}

EPDFStatusCode PDFDocumentCopyingContext::Start(IByteReaderWithPosition* inPDFStream,
											 DocumentsContext* inDocumentsContext,
											 ObjectsContext* inObjectsContext)
{
	mDocumentHandler.SetOperationsContexts(inDocumentsContext,inObjectsContext);
	return mDocumentHandler.StartStreamCopyingContext(inPDFStream);
}


EPDFStatusCodeAndObjectIDType PDFDocumentCopyingContext::CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
																					EPDFPageBox inPageBoxToUseAsFormBox,
																					const double* inTransformationMatrix)
{
	return mDocumentHandler.CreateFormXObjectFromPDFPage(inPageIndex,inPageBoxToUseAsFormBox,inTransformationMatrix);
}

EPDFStatusCodeAndObjectIDType PDFDocumentCopyingContext::CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
																					 const PDFRectangle& inCropBox,
																					 const double* inTransformationMatrix)
{
	return mDocumentHandler.CreateFormXObjectFromPDFPage(inPageIndex,inCropBox,inTransformationMatrix);	
}


EPDFStatusCodeAndObjectIDType PDFDocumentCopyingContext::AppendPDFPageFromPDF(unsigned long inPageIndex)
{
	return mDocumentHandler.AppendPDFPageFromPDF(inPageIndex);
}

EPDFStatusCodeAndObjectIDType PDFDocumentCopyingContext::CopyObject(ObjectIDType inSourceObjectID)
{
	return mDocumentHandler.CopyObject(inSourceObjectID);
}

EPDFStatusCodeAndObjectIDType PDFDocumentCopyingContext::GetCopiedObjectID(ObjectIDType inSourceObjectID)
{
	return mDocumentHandler.GetCopiedObjectID(inSourceObjectID);
}

HummusPDFParser* PDFDocumentCopyingContext::GetSourceDocumentParser()
{
	return mDocumentHandler.GetSourceDocumentParser();
}

MapIterator<ObjectIDTypeToObjectIDTypeMap> PDFDocumentCopyingContext::GetCopiedObjectsMappingIterator()
{
	return mDocumentHandler.GetCopiedObjectsMappingIterator();
}

EPDFStatusCode PDFDocumentCopyingContext::MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex)
{
	return mDocumentHandler.MergePDFPageToPage(inTargetPage,inSourcePageIndex);
}


void PDFDocumentCopyingContext::End()
{
	return mDocumentHandler.StopCopyingContext();
}

EPDFStatusCodeAndObjectIDTypeList PDFDocumentCopyingContext::CopyDirectObject(PDFObject* inObject)
{
	return mDocumentHandler.CopyDirectObject(inObject);
}

EPDFStatusCode PDFDocumentCopyingContext::CopyNewObjectsForDirectObject(const ObjectIDTypeList& inReferencedObjects)
{
	return mDocumentHandler.CopyNewObjectsForDirectObject(inReferencedObjects);
}

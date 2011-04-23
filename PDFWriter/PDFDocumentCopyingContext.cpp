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

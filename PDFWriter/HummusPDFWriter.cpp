/*
   Source File : PDFWriter.cpp


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

#include "HummusPDFWriter.h"
#include "Trace.h"
#include "Singleton.h"
#include "StateWriter.h"
#include "StateReader.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFIndirectObjectReference.h"
#include "IByteWriterWithPosition.h"

const LogConfiguration LogConfiguration::DefaultLogConfiguration(false,L"PDFWriterLog.txt");

const PDFCreationSettings PDFCreationSettings::DefaultPDFCreationSettings(true);

HummusPDFWriter::HummusPDFWriter(void)
{
	// Gal: some trick, already set reference to objects context. this does not mean that the document context can start doing something.
	// this allows the creation of copying context before starting to write the PDF, so that already
	// the first decision (level) about the PDF can be the result of parsing
	mDocumentsContext.SetObjectsContext(&mObjectsContext);
}

HummusPDFWriter::~HummusPDFWriter(void)
{
}

EPDFStatusCode HummusPDFWriter::StartPDF(
							const wstring& inOutputFilePath,
							EPDFVersion inPDFVersion,
							const LogConfiguration& inLogConfiguration,
							const PDFCreationSettings& inPDFCreationSettings)
{
	SetupLog(inLogConfiguration);
	SetupObjectsContext(inPDFCreationSettings);
	EPDFStatusCode status = mOutputFile.OpenFile(inOutputFilePath);
	if(status != ePDFSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentsContext.SetOutputFileInformation(&mOutputFile);
	
	return mDocumentsContext.WriteHeader(inPDFVersion);
}

EPDFStatusCode HummusPDFWriter::EndPDF()
{
	EPDFStatusCode status;
	do
	{
		status = mDocumentsContext.FinalizePDF();
		if(status != ePDFSuccess)
		{
			mOutputFile.CloseFile();
			TRACE_LOG("HummusPDFWriter::EndPDF, Could not end PDF");
			break;
		}
		status = mOutputFile.CloseFile();
	}
	while(false);
	ReleaseLog();
	return status;
}

EPDFStatusCodeAndObjectIDType HummusPDFWriter::WritePageAndReturnPageID(PDFPage* inPage)
{
	return mDocumentsContext.WritePage(inPage);
}

EPDFStatusCodeAndObjectIDType HummusPDFWriter::WritePageReleaseAndReturnPageID(PDFPage* inPage)
{
	return mDocumentsContext.WritePageAndRelease(inPage);
}

EPDFStatusCode HummusPDFWriter::WritePage(PDFPage* inPage)
{
	return mDocumentsContext.WritePage(inPage).first;
}

EPDFStatusCode HummusPDFWriter::WritePageAndRelease(PDFPage* inPage)
{
	return mDocumentsContext.WritePageAndRelease(inPage).first;
}


void HummusPDFWriter::SetupLog(const LogConfiguration& inLogConfiguration)
{
	if(inLogConfiguration.LogStream)
		Singleton<Trace>::GetInstance()->SetLogSettings(inLogConfiguration.LogStream,inLogConfiguration.ShouldLog);
	else
		Singleton<Trace>::GetInstance()->SetLogSettings(inLogConfiguration.LogFileLocation,inLogConfiguration.ShouldLog);
}

void HummusPDFWriter::SetupObjectsContext(const PDFCreationSettings& inPDFCreationSettings)
{
	mObjectsContext.SetCompressStreams(inPDFCreationSettings.CompressStreams);
}

void HummusPDFWriter::ReleaseLog()
{
	Singleton<Trace>::Reset();
}

DocumentsContext& HummusPDFWriter::GetDocumentsContext()
{
	return mDocumentsContext;
}

ObjectsContext& HummusPDFWriter::GetObjectsContext()
{
	return mObjectsContext;
}

OutputFile& HummusPDFWriter::GetOutputFile()
{
	return mOutputFile;
}

PageContentContext* HummusPDFWriter::StartPageContentContext(PDFPage* inPage)
{
	return mDocumentsContext.StartPageContentContext(inPage);
}

EPDFStatusCode HummusPDFWriter::PausePageContentContext(PageContentContext* inPageContext)
{
	return mDocumentsContext.PausePageContentContext(inPageContext);
}

EPDFStatusCode HummusPDFWriter::EndPageContentContext(PageContentContext* inPageContext)
{
	return mDocumentsContext.EndPageContentContext(inPageContext);
}

PDFFormXObject* HummusPDFWriter::StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix)
{
	return mDocumentsContext.StartFormXObject(inBoundingBox,inMatrix);
}

PDFFormXObject* HummusPDFWriter::StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix)
{
	return mDocumentsContext.StartFormXObject(inBoundingBox,inFormXObjectID,inMatrix);
}


EPDFStatusCode HummusPDFWriter::EndFormXObjectAndRelease(PDFFormXObject* inFormXObject)
{
	return mDocumentsContext.EndFormXObjectAndRelease(inFormXObject);
}

PDFImageXObject* HummusPDFWriter::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mDocumentsContext.CreateImageXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mDocumentsContext.CreateFormXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromTIFFFile(const wstring& inTIFFFilePath,const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentsContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inTIFFUsageParameters); 
}

PDFImageXObject* HummusPDFWriter::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentsContext.CreateImageXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentsContext.CreateFormXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromTIFFFile(const wstring& inTIFFFilePath,ObjectIDType inFormXObjectID, const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentsContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inFormXObjectID,inTIFFUsageParameters); 
}

PDFUsedFont* HummusPDFWriter::GetFontForFile(const wstring& inFontFilePath)
{
	return mDocumentsContext.GetFontForFile(inFontFilePath);
}

PDFUsedFont* HummusPDFWriter::GetFontForFile(const wstring& inFontFilePath,const wstring& inAdditionalMeticsFilePath)
{
	return mDocumentsContext.GetFontForFile(inFontFilePath,inAdditionalMeticsFilePath);
}

EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
																	  const PDFPageRange& inPageRange,
																	  EPDFPageBox inPageBoxToUseAsFormBox,
																	  const double* inTransformationMatrix,
																	  const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.CreateFormXObjectsFromPDF(inPDFFilePath,
														inPageRange,
														inPageBoxToUseAsFormBox,
														inTransformationMatrix,
														inCopyAdditionalObjects);
}

EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
																	 const PDFPageRange& inPageRange,
																	 const PDFRectangle& inCropBox,
																	 const double* inTransformationMatrix,
																	 const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.CreateFormXObjectsFromPDF(inPDFFilePath,
														inPageRange,
														inCropBox,
														inTransformationMatrix,
														inCopyAdditionalObjects);
}

EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.AppendPDFPagesFromPDF(inPDFFilePath,
														inPageRange,
														inCopyAdditionalObjects);
}

EPDFStatusCode HummusPDFWriter::Shutdown(const wstring& inStateFilePath)
{
	EPDFStatusCode status;

	do
	{
		StateWriter writer;

		status = writer.Start(inStateFilePath);
		if(status != ePDFSuccess)
		{
			TRACE_LOG("HummusPDFWriter::Shutdown, cant start state writing");
			break;
		}

		ObjectIDType rootObjectID = writer.GetObjectsWriter()->StartNewIndirectObject();
		DictionaryContext* pdfWriterDictionary = writer.GetObjectsWriter()->StartDictionary();

		pdfWriterDictionary->WriteKey("Type");
		pdfWriterDictionary->WriteNameValue("PDFWriter");

		ObjectIDType objectsContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();
		ObjectIDType DocumentsContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();

		pdfWriterDictionary->WriteKey("mObjectsContext");
		pdfWriterDictionary->WriteObjectReferenceValue(objectsContextID);

		pdfWriterDictionary->WriteKey("mDocumentsContext");
		pdfWriterDictionary->WriteObjectReferenceValue(DocumentsContextID);

		writer.GetObjectsWriter()->EndDictionary(pdfWriterDictionary);
		writer.GetObjectsWriter()->EndIndirectObject();

		writer.SetRootObject(rootObjectID);

		status = mObjectsContext.WriteState(writer.GetObjectsWriter(),objectsContextID);
		if(status != ePDFSuccess)
			break;

		status = mDocumentsContext.WriteState(writer.GetObjectsWriter(),DocumentsContextID);
		if(status != ePDFSuccess)
			break;

		status = writer.Finish();
		if(status != ePDFSuccess)
		{
			TRACE_LOG("HummusPDFWriter::Shutdown, cant finish state writing");
		}

	}while(false);

	if(status != ePDFSuccess)
	{
		mOutputFile.CloseFile();
		TRACE_LOG("HummusPDFWriter::Shutdown, Could not end PDF");
	}
	else
		status = mOutputFile.CloseFile();
	ReleaseLog();
	return status;
}

EPDFStatusCode HummusPDFWriter::ContinuePDF(const wstring& inOutputFilePath,
								   const wstring& inStateFilePath,
								   const LogConfiguration& inLogConfiguration)
{
	

	SetupLog(inLogConfiguration);
	EPDFStatusCode status = mOutputFile.OpenFile(inOutputFilePath,true);
	if(status != ePDFSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentsContext.SetOutputFileInformation(&mOutputFile);

	return SetupState(inStateFilePath);


}

EPDFStatusCode HummusPDFWriter::SetupState(const wstring& inStateFilePath)
{
	EPDFStatusCode status;

	do
	{
		StateReader reader;

		status = reader.Start(inStateFilePath);
		if(status != ePDFSuccess)
		{
			TRACE_LOG("HummusPDFWriter::SetupState, cant start state readering");
			break;
		}

		PDFObjectCastPtr<PDFDictionary> pdfWriterDictionary(reader.GetObjectsReader()->ParseNewObject(reader.GetRootObjectID()));

		PDFObjectCastPtr<PDFIndirectObjectReference> objectsContextObject(pdfWriterDictionary->QueryDirectObject("mObjectsContext"));
		status = mObjectsContext.ReadState(reader.GetObjectsReader(),objectsContextObject->mObjectID);
		if(status!= ePDFSuccess)
			break;

		PDFObjectCastPtr<PDFIndirectObjectReference> DocumentsContextObject(pdfWriterDictionary->QueryDirectObject("mDocumentsContext"));
		status = mDocumentsContext.ReadState(reader.GetObjectsReader(),DocumentsContextObject->mObjectID);
		if(status!= ePDFSuccess)
			break;

		reader.Finish();

	}while(false);

	return status;
}


EPDFStatusCode HummusPDFWriter::ContinuePDFForStream(IByteWriterWithPosition* inOutputStream,
											const wstring& inStateFilePath,
			 								const LogConfiguration& inLogConfiguration)
{
	SetupLog(inLogConfiguration);

	mObjectsContext.SetOutputStream(inOutputStream);

	return SetupState(inStateFilePath);

}


PDFDocumentCopyingContext* HummusPDFWriter::CreatePDFCopyingContext(const wstring& inPDFFilePath)
{
	return mDocumentsContext.CreatePDFCopyingContext(inPDFFilePath);
}

EPDFStatusCode HummusPDFWriter::AttachURLLinktoCurrentPage(const wstring& inURL,const PDFRectangle& inLinkClickArea)
{
	return mDocumentsContext.AttachURLLinktoCurrentPage(inURL,inLinkClickArea);
}

EPDFStatusCode HummusPDFWriter::MergePDFPagesToPage(PDFPage* inPage,
								const wstring& inPDFFilePath,
								const PDFPageRange& inPageRange,
								const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.MergePDFPagesToPage(inPage,
												inPDFFilePath,
												inPageRange,
												inCopyAdditionalObjects);
}


EPDFStatusCode HummusPDFWriter::StartPDFForStream(IByteWriterWithPosition* inOutputStream,
										 EPDFVersion inPDFVersion,
										 const LogConfiguration& inLogConfiguration,
										 const PDFCreationSettings& inPDFCreationSettings)
{
	SetupLog(inLogConfiguration);
	SetupObjectsContext(inPDFCreationSettings);

	mObjectsContext.SetOutputStream(inOutputStream);
	
	return mDocumentsContext.WriteHeader(inPDFVersion);
}
EPDFStatusCode HummusPDFWriter::EndPDFForStream()
{
	EPDFStatusCode status = mDocumentsContext.FinalizePDF();
	ReleaseLog();
	return status;
}

PDFImageXObject* HummusPDFWriter::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mDocumentsContext.CreateImageXObjectFromJPGStream(inJPGStream);
}

PDFImageXObject* HummusPDFWriter::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID)
{
	return mDocumentsContext.CreateImageXObjectFromJPGStream(inJPGStream,inImageXObjectID);
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mDocumentsContext.CreateFormXObjectFromJPGStream(inJPGStream);

}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID)
{
	return mDocumentsContext.CreateFormXObjectFromJPGStream(inJPGStream,inFormXObjectID);
}


PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
															const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentsContext.CreateFormXObjectFromTIFFStream(inTIFFStream,inTIFFUsageParameters);
}

PDFFormXObject* HummusPDFWriter::CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
															ObjectIDType inFormXObjectID,
															const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentsContext.CreateFormXObjectFromTIFFStream(inTIFFStream,inFormXObjectID,inTIFFUsageParameters);
}


EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																	const PDFPageRange& inPageRange,
																	EPDFPageBox inPageBoxToUseAsFormBox,
																	const double* inTransformationMatrix,
																	const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.CreateFormXObjectsFromPDF(inPDFStream,inPageRange,inPageBoxToUseAsFormBox,inTransformationMatrix,inCopyAdditionalObjects);
}

EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																	const PDFPageRange& inPageRange,
																	const PDFRectangle& inCropBox,
																	const double* inTransformationMatrix,
																	const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.CreateFormXObjectsFromPDF(inPDFStream,inPageRange,inCropBox,inTransformationMatrix,inCopyAdditionalObjects);
}

EPDFStatusCodeAndObjectIDTypeList HummusPDFWriter::AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
																const PDFPageRange& inPageRange,
																const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.AppendPDFPagesFromPDF(inPDFStream,inPageRange,inCopyAdditionalObjects);
}

EPDFStatusCode HummusPDFWriter::MergePDFPagesToPage(	PDFPage* inPage,
											IByteReaderWithPosition* inPDFStream,
											const PDFPageRange& inPageRange,
											const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentsContext.MergePDFPagesToPage(inPage,inPDFStream,inPageRange,inCopyAdditionalObjects);
}

PDFDocumentCopyingContext* HummusPDFWriter::CreatePDFCopyingContext(IByteReaderWithPosition* inPDFStream)
{
	return mDocumentsContext.CreatePDFCopyingContext(inPDFStream);	
}


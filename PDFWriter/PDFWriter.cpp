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

#include "PDFWriter.h"
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

using namespace PDFHummus;

const LogConfiguration LogConfiguration::DefaultLogConfiguration(false,false,"PDFWriterLog.txt");

const PDFCreationSettings PDFCreationSettings::DefaultPDFCreationSettings(true);

PDFWriter::PDFWriter(void)
{
	// Gal: some trick, already set reference to objects context. this does not mean that the document context can start doing something.
	// this allows the creation of copying context before starting to write the PDF, so that already
	// the first decision (level) about the PDF can be the result of parsing
	mDocumentContext.SetObjectsContext(&mObjectsContext);
}

PDFWriter::~PDFWriter(void)
{
}

EStatusCode PDFWriter::StartPDF(
							const string& inOutputFilePath,
							EPDFVersion inPDFVersion,
							const LogConfiguration& inLogConfiguration,
							const PDFCreationSettings& inPDFCreationSettings)
{
	SetupLog(inLogConfiguration);
	SetupObjectsContext(inPDFCreationSettings);
	EStatusCode status = mOutputFile.OpenFile(inOutputFilePath);
	if(status != PDFHummus::eSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentContext.SetOutputFileInformation(&mOutputFile);
	
	return mDocumentContext.WriteHeader(inPDFVersion);
}

EStatusCode PDFWriter::EndPDF()
{
	EStatusCode status;
	do
	{
		status = mDocumentContext.FinalizePDF();
		if(status != PDFHummus::eSuccess)
		{
			mOutputFile.CloseFile();
			TRACE_LOG("PDFWriter::EndPDF, Could not end PDF");
			break;
		}
		status = mOutputFile.CloseFile();
	}
	while(false);
	Cleanup();
	return status;
}

void PDFWriter::Cleanup()
{
	mObjectsContext.Cleanup();
	mDocumentContext.Cleanup();
	ReleaseLog();
}

void PDFWriter::Reset()
{
	mOutputFile.CloseFile();
	Cleanup();
}

EStatusCodeAndObjectIDType PDFWriter::WritePageAndReturnPageID(PDFPage* inPage)
{
	return mDocumentContext.WritePage(inPage);
}

EStatusCodeAndObjectIDType PDFWriter::WritePageReleaseAndReturnPageID(PDFPage* inPage)
{
	return mDocumentContext.WritePageAndRelease(inPage);
}

EStatusCode PDFWriter::WritePage(PDFPage* inPage)
{
	return mDocumentContext.WritePage(inPage).first;
}

EStatusCode PDFWriter::WritePageAndRelease(PDFPage* inPage)
{
	return mDocumentContext.WritePageAndRelease(inPage).first;
}


void PDFWriter::SetupLog(const LogConfiguration& inLogConfiguration)
{
	if(inLogConfiguration.LogStream)
		Singleton<Trace>::GetInstance()->SetLogSettings(inLogConfiguration.LogStream,inLogConfiguration.ShouldLog);
	else
		Singleton<Trace>::GetInstance()->SetLogSettings(inLogConfiguration.LogFileLocation,inLogConfiguration.ShouldLog,inLogConfiguration.StartWithBOM);
}

void PDFWriter::SetupObjectsContext(const PDFCreationSettings& inPDFCreationSettings)
{
	mObjectsContext.SetCompressStreams(inPDFCreationSettings.CompressStreams);
}

void PDFWriter::ReleaseLog()
{
	Singleton<Trace>::Reset();
}

DocumentContext& PDFWriter::GetDocumentContext()
{
	return mDocumentContext;
}

ObjectsContext& PDFWriter::GetObjectsContext()
{
	return mObjectsContext;
}

OutputFile& PDFWriter::GetOutputFile()
{
	return mOutputFile;
}

PageContentContext* PDFWriter::StartPageContentContext(PDFPage* inPage)
{
	return mDocumentContext.StartPageContentContext(inPage);
}

EStatusCode PDFWriter::PausePageContentContext(PageContentContext* inPageContext)
{
	return mDocumentContext.PausePageContentContext(inPageContext);
}

EStatusCode PDFWriter::EndPageContentContext(PageContentContext* inPageContext)
{
	return mDocumentContext.EndPageContentContext(inPageContext);
}

PDFFormXObject* PDFWriter::StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix)
{
	return mDocumentContext.StartFormXObject(inBoundingBox,inMatrix);
}

PDFFormXObject* PDFWriter::StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix)
{
	return mDocumentContext.StartFormXObject(inBoundingBox,inFormXObjectID,inMatrix);
}


EStatusCode PDFWriter::EndFormXObjectAndRelease(PDFFormXObject* inFormXObject)
{
	return mDocumentContext.EndFormXObjectAndRelease(inFormXObject);
}

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGFile(const string& inJPGFilePath)
{
	return mDocumentContext.CreateImageXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGFile(const string& inJPGFilePath)
{
	return mDocumentContext.CreateFormXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFFile(const string& inTIFFFilePath,const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inTIFFUsageParameters); 
}

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentContext.CreateImageXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentContext.CreateFormXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFFile(const string& inTIFFFilePath,ObjectIDType inFormXObjectID, const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inFormXObjectID,inTIFFUsageParameters); 
}

PDFUsedFont* PDFWriter::GetFontForFile(const string& inFontFilePath)
{
	return mDocumentContext.GetFontForFile(inFontFilePath);
}

PDFUsedFont* PDFWriter::GetFontForFile(const string& inFontFilePath,const string& inAdditionalMeticsFilePath)
{
	return mDocumentContext.GetFontForFile(inFontFilePath,inAdditionalMeticsFilePath);
}

EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(const string& inPDFFilePath,
																	  const PDFPageRange& inPageRange,
																	  EPDFPageBox inPageBoxToUseAsFormBox,
																	  const double* inTransformationMatrix,
																	  const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.CreateFormXObjectsFromPDF(inPDFFilePath,
														inPageRange,
														inPageBoxToUseAsFormBox,
														inTransformationMatrix,
														inCopyAdditionalObjects);
}

EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(const string& inPDFFilePath,
																	 const PDFPageRange& inPageRange,
																	 const PDFRectangle& inCropBox,
																	 const double* inTransformationMatrix,
																	 const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.CreateFormXObjectsFromPDF(inPDFFilePath,
														inPageRange,
														inCropBox,
														inTransformationMatrix,
														inCopyAdditionalObjects);
}

EStatusCodeAndObjectIDTypeList PDFWriter::AppendPDFPagesFromPDF(const string& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.AppendPDFPagesFromPDF(inPDFFilePath,
														inPageRange,
														inCopyAdditionalObjects);
}

EStatusCode PDFWriter::Shutdown(const string& inStateFilePath)
{
	EStatusCode status;

	do
	{
		StateWriter writer;

		status = writer.Start(inStateFilePath);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("PDFWriter::Shutdown, cant start state writing");
			break;
		}

		ObjectIDType rootObjectID = writer.GetObjectsWriter()->StartNewIndirectObject();
		DictionaryContext* pdfWriterDictionary = writer.GetObjectsWriter()->StartDictionary();

		pdfWriterDictionary->WriteKey("Type");
		pdfWriterDictionary->WriteNameValue("PDFWriter");

		ObjectIDType objectsContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();
		ObjectIDType DocumentContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();

		pdfWriterDictionary->WriteKey("mObjectsContext");
		pdfWriterDictionary->WriteObjectReferenceValue(objectsContextID);

		pdfWriterDictionary->WriteKey("mDocumentContext");
		pdfWriterDictionary->WriteObjectReferenceValue(DocumentContextID);

		writer.GetObjectsWriter()->EndDictionary(pdfWriterDictionary);
		writer.GetObjectsWriter()->EndIndirectObject();

		writer.SetRootObject(rootObjectID);

		status = mObjectsContext.WriteState(writer.GetObjectsWriter(),objectsContextID);
		if(status != PDFHummus::eSuccess)
			break;

		status = mDocumentContext.WriteState(writer.GetObjectsWriter(),DocumentContextID);
		if(status != PDFHummus::eSuccess)
			break;

		status = writer.Finish();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("PDFWriter::Shutdown, cant finish state writing");
		}

	}while(false);

	if(status != PDFHummus::eSuccess)
	{
		mOutputFile.CloseFile();
		TRACE_LOG("PDFWriter::Shutdown, Could not end PDF");
	}
	else
		status = mOutputFile.CloseFile();
	ReleaseLog();
	return status;
}

EStatusCode PDFWriter::ContinuePDF(const string& inOutputFilePath,
								   const string& inStateFilePath,
								   const LogConfiguration& inLogConfiguration)
{
	

	SetupLog(inLogConfiguration);
	EStatusCode status = mOutputFile.OpenFile(inOutputFilePath,true);
	if(status != PDFHummus::eSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentContext.SetOutputFileInformation(&mOutputFile);

	return SetupState(inStateFilePath);


}

EStatusCode PDFWriter::SetupState(const string& inStateFilePath)
{
	EStatusCode status;

	do
	{
		StateReader reader;

		status = reader.Start(inStateFilePath);
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("PDFWriter::SetupState, cant start state readering");
			break;
		}

		PDFObjectCastPtr<PDFDictionary> pdfWriterDictionary(reader.GetObjectsReader()->ParseNewObject(reader.GetRootObjectID()));

		PDFObjectCastPtr<PDFIndirectObjectReference> objectsContextObject(pdfWriterDictionary->QueryDirectObject("mObjectsContext"));
		status = mObjectsContext.ReadState(reader.GetObjectsReader(),objectsContextObject->mObjectID);
		if(status!= PDFHummus::eSuccess)
			break;

		PDFObjectCastPtr<PDFIndirectObjectReference> DocumentContextObject(pdfWriterDictionary->QueryDirectObject("mDocumentContext"));
		status = mDocumentContext.ReadState(reader.GetObjectsReader(),DocumentContextObject->mObjectID);
		if(status!= PDFHummus::eSuccess)
			break;

		reader.Finish();

	}while(false);

	return status;
}


EStatusCode PDFWriter::ContinuePDFForStream(IByteWriterWithPosition* inOutputStream,
											const string& inStateFilePath,
			 								const LogConfiguration& inLogConfiguration)
{
	SetupLog(inLogConfiguration);

	mObjectsContext.SetOutputStream(inOutputStream);

	return SetupState(inStateFilePath);

}


PDFDocumentCopyingContext* PDFWriter::CreatePDFCopyingContext(const string& inPDFFilePath)
{
	return mDocumentContext.CreatePDFCopyingContext(inPDFFilePath);
}

EStatusCode PDFWriter::AttachURLLinktoCurrentPage(const string& inURL,const PDFRectangle& inLinkClickArea)
{
	return mDocumentContext.AttachURLLinktoCurrentPage(inURL,inLinkClickArea);
}

EStatusCode PDFWriter::MergePDFPagesToPage(PDFPage* inPage,
								const string& inPDFFilePath,
								const PDFPageRange& inPageRange,
								const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.MergePDFPagesToPage(inPage,
												inPDFFilePath,
												inPageRange,
												inCopyAdditionalObjects);
}


EStatusCode PDFWriter::StartPDFForStream(IByteWriterWithPosition* inOutputStream,
										 EPDFVersion inPDFVersion,
										 const LogConfiguration& inLogConfiguration,
										 const PDFCreationSettings& inPDFCreationSettings)
{
	SetupLog(inLogConfiguration);
	SetupObjectsContext(inPDFCreationSettings);

	mObjectsContext.SetOutputStream(inOutputStream);
	
	return mDocumentContext.WriteHeader(inPDFVersion);
}
EStatusCode PDFWriter::EndPDFForStream()
{
	EStatusCode status = mDocumentContext.FinalizePDF();
	Cleanup();
	return status;
}

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mDocumentContext.CreateImageXObjectFromJPGStream(inJPGStream);
}

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID)
{
	return mDocumentContext.CreateImageXObjectFromJPGStream(inJPGStream,inImageXObjectID);
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream)
{
	return mDocumentContext.CreateFormXObjectFromJPGStream(inJPGStream);

}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID)
{
	return mDocumentContext.CreateFormXObjectFromJPGStream(inJPGStream,inFormXObjectID);
}


PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
															const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFStream(inTIFFStream,inTIFFUsageParameters);
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
															ObjectIDType inFormXObjectID,
															const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFStream(inTIFFStream,inFormXObjectID,inTIFFUsageParameters);
}


EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																	const PDFPageRange& inPageRange,
																	EPDFPageBox inPageBoxToUseAsFormBox,
																	const double* inTransformationMatrix,
																	const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.CreateFormXObjectsFromPDF(inPDFStream,inPageRange,inPageBoxToUseAsFormBox,inTransformationMatrix,inCopyAdditionalObjects);
}

EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
																	const PDFPageRange& inPageRange,
																	const PDFRectangle& inCropBox,
																	const double* inTransformationMatrix,
																	const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.CreateFormXObjectsFromPDF(inPDFStream,inPageRange,inCropBox,inTransformationMatrix,inCopyAdditionalObjects);
}

EStatusCodeAndObjectIDTypeList PDFWriter::AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
																const PDFPageRange& inPageRange,
																const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.AppendPDFPagesFromPDF(inPDFStream,inPageRange,inCopyAdditionalObjects);
}

EStatusCode PDFWriter::MergePDFPagesToPage(	PDFPage* inPage,
											IByteReaderWithPosition* inPDFStream,
											const PDFPageRange& inPageRange,
											const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.MergePDFPagesToPage(inPage,inPDFStream,inPageRange,inCopyAdditionalObjects);
}

PDFDocumentCopyingContext* PDFWriter::CreatePDFCopyingContext(IByteReaderWithPosition* inPDFStream)
{
	return mDocumentContext.CreatePDFCopyingContext(inPDFStream);	
}


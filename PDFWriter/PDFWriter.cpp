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

const LogConfiguration LogConfiguration::DefaultLogConfiguration(false,L"PDFWriterLog.txt");

const PDFCreationSettings PDFCreationSettings::DefaultPDFCreationSettings(true);

PDFWriter::PDFWriter(void)
{
}

PDFWriter::~PDFWriter(void)
{
}

EStatusCode PDFWriter::StartPDF(
							const wstring& inOutputFilePath,
							EPDFVersion inPDFVersion,
							const LogConfiguration& inLogConfiguration,
							const PDFCreationSettings& inPDFCreationSettings)
{
	SetupLog(inLogConfiguration);
	SetupObjectsContext(inPDFCreationSettings);
	EStatusCode status = mOutputFile.OpenFile(inOutputFilePath);
	if(status != eSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentContext.SetObjectsContext(&mObjectsContext);
	mDocumentContext.SetOutputFileInformation(&mOutputFile);
	
	return mDocumentContext.WriteHeader(inPDFVersion);
}

EStatusCode PDFWriter::EndPDF()
{
	EStatusCode status;
	do
	{
		status = mDocumentContext.FinalizePDF();
		if(status != eSuccess)
		{
			mOutputFile.CloseFile();
			TRACE_LOG("PDFWriter::EndPDF, Could not end PDF");
			break;
		}
		status = mOutputFile.CloseFile();
	}
	while(false);
	ReleaseLog();
	return status;
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
	Singleton<Trace>::GetInstance()->SetLogSettings(inLogConfiguration.LogFileLocation,inLogConfiguration.ShouldLog);
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

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mDocumentContext.CreateImageXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	return mDocumentContext.CreateFormXObjectFromJPGFile(inJPGFilePath); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFFile(const wstring& inTIFFFilePath,const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inTIFFUsageParameters); 
}

PDFImageXObject* PDFWriter::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentContext.CreateImageXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	return mDocumentContext.CreateFormXObjectFromJPGFile(inJPGFilePath,inImageXObjectID); 
}

PDFFormXObject* PDFWriter::CreateFormXObjectFromTIFFFile(const wstring& inTIFFFilePath,ObjectIDType inFormXObjectID, const TIFFUsageParameters& inTIFFUsageParameters)
{
	return mDocumentContext.CreateFormXObjectFromTIFFFile(inTIFFFilePath,inFormXObjectID,inTIFFUsageParameters); 
}

PDFUsedFont* PDFWriter::GetFontForFile(const wstring& inFontFilePath)
{
	return mDocumentContext.GetFontForFile(inFontFilePath);
}

PDFUsedFont* PDFWriter::GetFontForFile(const wstring& inFontFilePath,const wstring& inAdditionalMeticsFilePath)
{
	return mDocumentContext.GetFontForFile(inFontFilePath,inAdditionalMeticsFilePath);
}

EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
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

EStatusCodeAndObjectIDTypeList PDFWriter::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
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

EStatusCodeAndObjectIDTypeList PDFWriter::AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange,
																const ObjectIDTypeList& inCopyAdditionalObjects)
{
	return mDocumentContext.AppendPDFPagesFromPDF(inPDFFilePath,
														inPageRange,
														inCopyAdditionalObjects);
}

EStatusCode PDFWriter::Shutdown(const wstring& inStateFilePath)
{
	EStatusCode status;

	do
	{
		StateWriter writer;

		status = writer.Start(inStateFilePath);
		if(status != eSuccess)
		{
			TRACE_LOG("PDFWriter::Shutdown, cant start state writing");
			break;
		}

		ObjectIDType rootObjectID = writer.GetObjectsWriter()->StartNewIndirectObject();
		DictionaryContext* pdfWriterDictionary = writer.GetObjectsWriter()->StartDictionary();

		pdfWriterDictionary->WriteKey("Type");
		pdfWriterDictionary->WriteNameValue("PDFWriter");

		ObjectIDType objectsContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();
		ObjectIDType documentContextID = writer.GetObjectsWriter()->GetInDirectObjectsRegistry().AllocateNewObjectID();

		pdfWriterDictionary->WriteKey("mObjectsContext");
		pdfWriterDictionary->WriteObjectReferenceValue(objectsContextID);

		pdfWriterDictionary->WriteKey("mDocumentContext");
		pdfWriterDictionary->WriteObjectReferenceValue(documentContextID);

		writer.GetObjectsWriter()->EndDictionary(pdfWriterDictionary);
		writer.GetObjectsWriter()->EndIndirectObject();

		writer.SetRootObject(rootObjectID);

		status = mObjectsContext.WriteState(writer.GetObjectsWriter(),objectsContextID);
		if(status != eSuccess)
			break;

		status = mDocumentContext.WriteState(writer.GetObjectsWriter(),documentContextID);
		if(status != eSuccess)
			break;

		status = writer.Finish();
		if(status != eSuccess)
		{
			TRACE_LOG("PDFWriter::Shutdown, cant finish state writing");
		}

	}while(false);

	if(status != eSuccess)
	{
		mOutputFile.CloseFile();
		TRACE_LOG("PDFWriter::Shutdown, Could not end PDF");
	}
	else
		status = mOutputFile.CloseFile();
	ReleaseLog();
	return status;
}

EStatusCode PDFWriter::ContinuePDF(const wstring& inOutputFilePath,
								   const wstring& inStateFilePath,
								   const LogConfiguration& inLogConfiguration)
{
	SetupLog(inLogConfiguration);
	EStatusCode status = mOutputFile.OpenFile(inOutputFilePath,true);
	if(status != eSuccess)
		return status;

	mObjectsContext.SetOutputStream(mOutputFile.GetOutputStream());
	mDocumentContext.SetObjectsContext(&mObjectsContext);
	mDocumentContext.SetOutputFileInformation(&mOutputFile);

	do
	{
		StateReader reader;

		status = reader.Start(inStateFilePath);
		if(status != eSuccess)
		{
			TRACE_LOG("PDFWriter::ContinuePDF, cant start state readering");
			break;
		}

		PDFObjectCastPtr<PDFDictionary> pdfWriterDictionary(reader.GetObjectsReader()->ParseNewObject(reader.GetRootObjectID()));

		PDFObjectCastPtr<PDFIndirectObjectReference> objectsContextObject(pdfWriterDictionary->QueryDirectObject("mObjectsContext"));
		status = mObjectsContext.ReadState(reader.GetObjectsReader(),objectsContextObject->mObjectID);
		if(status!= eSuccess)
			break;

		PDFObjectCastPtr<PDFIndirectObjectReference> documentContextObject(pdfWriterDictionary->QueryDirectObject("mDocumentContext"));
		status = mDocumentContext.ReadState(reader.GetObjectsReader(),documentContextObject->mObjectID);
		if(status!= eSuccess)
			break;

		reader.Finish();

	}while(false);

	return status;
}

PDFDocumentCopyingContext* PDFWriter::CreatePDFCopyingContext(const wstring& inPDFFilePath)
{
	return mDocumentContext.CreatePDFCopyingContext(inPDFFilePath);
}

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

EStatusCodeAndPDFFormXObjectList PDFWriter::CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
																	  const PDFPageRange& inPageRange,
																	  EPDFPageBox inPageBoxToUseAsFormBox,
																	  const double* inTransformationMatrix)
{
	return mDocumentContext.CreateFormXObjectsFromPDF(inPDFFilePath,
														inPageRange,
														inPageBoxToUseAsFormBox,
														inTransformationMatrix);
}

EStatusCodeAndObjectIDTypeList PDFWriter::AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
																const PDFPageRange& inPageRange)
{
	return mDocumentContext.AppendPDFPagesFromPDF(inPDFFilePath,
														inPageRange);
}


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

EStatusCode PDFWriter::WritePage(PDFPage* inPage)
{
	return mDocumentContext.WritePage(inPage);
}

EStatusCode PDFWriter::WritePageAndRelease(PDFPage* inPage)
{
	return mDocumentContext.WritePageAndRelease(inPage);
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
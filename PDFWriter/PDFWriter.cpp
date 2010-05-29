#include "PDFWriter.h"
#include "Trace.h"
#include "Singleton.h"

const LogConfiguration LogConfiguration::DefaultLogConfiguration(false,L"PDFWriterLog.txt");


PDFWriter::PDFWriter(void)
{
}

PDFWriter::~PDFWriter(void)
{
}


EStatusCode PDFWriter::InitializePDFWriter(const wstring& inOutputFilePath,const LogConfiguration& inLogConfiguration)
{
	SetupLog(inLogConfiguration);
	return mOutputFile.OpenFile(inOutputFilePath);
}

EStatusCode PDFWriter::StartPDF(EPDFVersion inPDFVersion)
{
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
			TRACE_LOG("PDFWriter::EndPDF, Could not end PDF");
		}

		status = mOutputFile.CloseFile();

		ReleaseLog();
	}
	while(false);
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
#pragma once
/*
	Main class for PDF library. entry point for writing PDFs.


*/

#include "EPDFVersion.h"
#include "OutputFile.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"

#include <string>

struct LogConfiguration
{
	bool ShouldLog;
	wstring LogFileLocation;

	LogConfiguration(bool inShouldLog,const wstring& inLogFileLocation){ShouldLog=inShouldLog;
																		LogFileLocation=inLogFileLocation;}

	static const LogConfiguration DefaultLogConfiguration;
};

struct PDFCreationSettings
{
	bool CompressStreams;

	PDFCreationSettings(bool inCompressStreams){CompressStreams = inCompressStreams;}

	static const PDFCreationSettings DefaultPDFCreationSettings;
};

class PageContentContext;
class PDFFormXObject;

class PDFWriter
{
public:
	PDFWriter(void);
	~PDFWriter(void);

	EStatusCode InitializePDFWriter(const wstring& inOutputFilePath,
									const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration,
									const PDFCreationSettings& inPDFCreationSettings = PDFCreationSettings::DefaultPDFCreationSettings);
	
	EStatusCode StartPDF(EPDFVersion inPDFVersion);
	EStatusCode EndPDF();


	// Page context, for drwaing page content
	PageContentContext* StartPageContentContext(PDFPage* inPage);
	EStatusCode PausePageContentContext(PageContentContext* inPageContext);
	EStatusCode EndPageContentContext(PageContentContext* inPageContext);


	// Page Writing [create a new Page by creating a new instance of PDFPage. instances may be reused.
	EStatusCode WritePage(PDFPage* inPage);
	EStatusCode WritePageAndRelease(PDFPage* inPage);


	// Form XObject creating and writing
	PDFFormXObject* CreateFormXObject();
	EStatusCode WriteFormXObjectAndRelease(PDFFormXObject* inFormXObject);

	// Extensibility, reaching to lower levels
	DocumentContext& GetDocumentContext();
	ObjectsContext& GetObjectsContext();
	OutputFile& GetOutputFile();
private:

	OutputFile mOutputFile;
	ObjectsContext mObjectsContext;
	DocumentContext mDocumentContext;


	void SetupLog(const LogConfiguration& inLogConfiguration);
	void SetupObjectsContext(const PDFCreationSettings& inPDFCreationSettings);
	void ReleaseLog();

};

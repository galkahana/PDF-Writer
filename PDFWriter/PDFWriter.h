#pragma once
/*
	Main class for PDF library. entry point for writing PDFs.


*/

#include "EPDFVersion.h"
#include "OutputFile.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "PDFRectangle.h"
#include "TIFFUsageParameters.h"

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
class PDFImageXObject;

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
	PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix = NULL);
	PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix = NULL);
	EStatusCode EndFormXObjectAndRelease(PDFFormXObject* inFormXObject);

	// Image XObject creating [for TIFF nad JPG files]. 
	// note that as oppose to other methods, create the image xobject also writes it, so there's no "WriteXXXXAndRelease" for image.
	// So...release the object yourself [just delete it]
	
	// jpeg - two variants
	// will return image xobject sized at 1X1
	PDFImageXObject* CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath);
	PDFImageXObject* CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID);

	// will return form XObject, which will include the xobject at it's size.
	// size is determined by the following order:
	// - JFIF resolution information is looked for. if found used to determine the size
	// - if not found. EXIF resolution information is looked for. if found used to determine the size
	// - if not found. Photoshop resolution information is looked for. if found used to determine the size
	// - otherwise aspect ratio is assumed, and so size is determined trivially from the samples width and height.
	PDFFormXObject* CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath);
	PDFFormXObject* CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inFormXObjectID);
	
	// tiff
	PDFFormXObject* CreateFormXObjectFromTIFFFile(	const wstring& inTIFFFilePath,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
	PDFFormXObject* CreateFormXObjectFromTIFFFile(	const wstring& inTIFFFilePath,
													ObjectIDType inFormXObjectID,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);

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

/*
   Source File : PDFWriter.h


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
#include "PDFEmbedParameterTypes.h"

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
class PDFUsedFont;

class PDFWriter
{
public:
	PDFWriter(void);
	~PDFWriter(void);

	EStatusCode StartPDF(	const wstring& inOutputFilePath,
							EPDFVersion inPDFVersion,
							const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration,
							const PDFCreationSettings& inPDFCreationSettings = PDFCreationSettings::DefaultPDFCreationSettings);
	EStatusCode EndPDF();

	// Ending and Restarting writing session
	EStatusCode Shutdown(const wstring& inStateFilePath);
	EStatusCode ContinuePDF(const wstring& inOutputFilePath,
							const wstring& inStateFilePath,
							const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration);


	// Page context, for drwaing page content
	PageContentContext* StartPageContentContext(PDFPage* inPage);
	EStatusCode PausePageContentContext(PageContentContext* inPageContext);
	EStatusCode EndPageContentContext(PageContentContext* inPageContext);


	// Page Writing [create a new Page by creating a new instance of PDFPage. instances may be reused.
	EStatusCode WritePage(PDFPage* inPage);
	EStatusCode WritePageAndRelease(PDFPage* inPage);

	// same as above page writing, but also return page ID. good for extensibility, when you want to refer to the written page form some other place
	EStatusCodeAndObjectIDType WritePageAndReturnPageID(PDFPage* inPage);
	EStatusCodeAndObjectIDType WritePageReleaseAndReturnPageID(PDFPage* inPage);


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

	// PDF 

	// CreateFormXObjectsFromPDF is for using input PDF pages as objects in one page or more. you can used the returned IDs to place the 
	// created form xobjects
	EStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const wstring& inPDFFilePath,
															 const PDFPageRange& inPageRange,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL,
															 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	// AppendPDFPagesFromPDF is for simple appending of the input PDF pages
	EStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const wstring& inPDFFilePath,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	// Copying context, allowing for a continous flow of copying from multiple sources PDFs (create one per source) to target PDF
	PDFDocumentCopyingContext* CreatePDFCopyingContext(const wstring& inPDFFilePath);

	// fonts [text]
	PDFUsedFont* GetFontForFile(const wstring& inFontFilePath);
	// second overload is for type 1, when an additional metrics file is available
	PDFUsedFont* GetFontForFile(const wstring& inFontFilePath,const wstring& inAdditionalMeticsFilePath);


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

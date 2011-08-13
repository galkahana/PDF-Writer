/*
   Source File : HummusPDFWriter.h


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
#include "DocumentsContext.h"
#include "ObjectsContext.h"
#include "PDFRectangle.h"
#include "TIFFUsageParameters.h"
#include "PDFEmbedParameterTypes.h"

#include <string>

struct LogConfiguration
{
	bool ShouldLog;
	bool StartWithBOM;
	string LogFileLocation;
	IByteWriter* LogStream;

	LogConfiguration(bool inShouldLog,bool inStartWithBOM,const string& inLogFileLocation){ShouldLog=inShouldLog;StartWithBOM = inStartWithBOM;
																							LogFileLocation=inLogFileLocation;LogStream = NULL;}
	LogConfiguration(bool inShouldLog,IByteWriter* inLogStream){ShouldLog = inShouldLog;LogStream = inLogStream;StartWithBOM = false;}

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
class IByteWriterWithPosition;

class HummusPDFWriter
{
public:
	HummusPDFWriter(void);
	~HummusPDFWriter(void);

	// output to file
	EPDFStatusCode StartPDF(const string& inOutputFilePath,
							EPDFVersion inPDFVersion,
							const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration,
							const PDFCreationSettings& inPDFCreationSettings = PDFCreationSettings::DefaultPDFCreationSettings);

	EPDFStatusCode EndPDF();

	// output to stream
	EPDFStatusCode StartPDFForStream(IByteWriterWithPosition* inOutputStream,
								  EPDFVersion inPDFVersion,
								  const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration,
								  const PDFCreationSettings& inPDFCreationSettings = PDFCreationSettings::DefaultPDFCreationSettings);
	EPDFStatusCode EndPDFForStream();


	// Ending and Restarting writing session
	EPDFStatusCode Shutdown(const string& inStateFilePath);
	EPDFStatusCode ContinuePDF(const string& inOutputFilePath,
							const string& inStateFilePath,
							const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration);
	// Continue PDF in output stream workflow
	EPDFStatusCode ContinuePDFForStream(IByteWriterWithPosition* inOutputStream,
									 const string& inStateFilePath,
				 					 const LogConfiguration& inLogConfiguration = LogConfiguration::DefaultLogConfiguration);

	// Page context, for drwaing page content
	PageContentContext* StartPageContentContext(PDFPage* inPage);
	EPDFStatusCode PausePageContentContext(PageContentContext* inPageContext);
	EPDFStatusCode EndPageContentContext(PageContentContext* inPageContext);


	// Page Writing [create a new Page by creating a new instance of PDFPage. instances may be reused.
	EPDFStatusCode WritePage(PDFPage* inPage);
	EPDFStatusCode WritePageAndRelease(PDFPage* inPage);

	// same as above page writing, but also return page ID. good for extensibility, when you want to refer to the written page form some other place
	EPDFStatusCodeAndObjectIDType WritePageAndReturnPageID(PDFPage* inPage);
	EPDFStatusCodeAndObjectIDType WritePageReleaseAndReturnPageID(PDFPage* inPage);


	// Form XObject creating and writing
	PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,const double* inMatrix = NULL);
	PDFFormXObject* StartFormXObject(const PDFRectangle& inBoundingBox,ObjectIDType inFormXObjectID,const double* inMatrix = NULL);
	EPDFStatusCode EndFormXObjectAndRelease(PDFFormXObject* inFormXObject);

	// Image XObject creating [for TIFF nad JPG files]. 
	// note that as oppose to other methods, create the image xobject also writes it, so there's no "WriteXXXXAndRelease" for image.
	// So...release the object yourself [just delete it]
	
	// jpeg - two variants
	// will return image xobject sized at 1X1
	PDFImageXObject* CreateImageXObjectFromJPGFile(const string& inJPGFilePath);
	PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
	PDFImageXObject* CreateImageXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inImageXObjectID);
	PDFImageXObject* CreateImageXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inImageXObjectID);

	// will return form XObject, which will include the xobject at it's size.
	// size is determined by the following order:
	// - JFIF resolution information is looked for. if found used to determine the size
	// - if not found. EXIF resolution information is looked for. if found used to determine the size
	// - if not found. Photoshop resolution information is looked for. if found used to determine the size
	// - otherwise aspect ratio is assumed, and so size is determined trivially from the samples width and height.
	PDFFormXObject* CreateFormXObjectFromJPGFile(const string& inJPGFilePath);
	PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream);
	PDFFormXObject* CreateFormXObjectFromJPGFile(const string& inJPGFilePath,ObjectIDType inFormXObjectID);
	PDFFormXObject* CreateFormXObjectFromJPGStream(IByteReaderWithPosition* inJPGStream,ObjectIDType inFormXObjectID);
	
	// tiff
	PDFFormXObject* CreateFormXObjectFromTIFFFile(	const string& inTIFFFilePath,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
	PDFFormXObject* CreateFormXObjectFromTIFFStream(IByteReaderWithPosition* inTIFFStream,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
	PDFFormXObject* CreateFormXObjectFromTIFFFile(	const string& inTIFFFilePath,
													ObjectIDType inFormXObjectID,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);
	PDFFormXObject* CreateFormXObjectFromTIFFStream(	IByteReaderWithPosition* inTIFFStream,
													ObjectIDType inFormXObjectID,
													const TIFFUsageParameters& inTIFFUsageParameters = TIFFUsageParameters::DefaultTIFFUsageParameters);

	// PDF 

	// CreateFormXObjectsFromPDF is for using input PDF pages as objects in one page or more. you can used the returned IDs to place the 
	// created form xobjects
	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const string& inPDFFilePath,
															 const PDFPageRange& inPageRange,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL,
															 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL,
															 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());
	
	// CreateFormXObjectsFromPDF is an override to allow you to determine a custom crop for the page embed
	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(const string& inPDFFilePath,
															 const PDFPageRange& inPageRange,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix = NULL,
															 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	EPDFStatusCodeAndObjectIDTypeList CreateFormXObjectsFromPDF(IByteReaderWithPosition* inPDFStream,
															 const PDFPageRange& inPageRange,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix = NULL,
															 const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	// AppendPDFPagesFromPDF is for simple appending of the input PDF pages
	EPDFStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(const string& inPDFFilePath,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());
	
	EPDFStatusCodeAndObjectIDTypeList AppendPDFPagesFromPDF(IByteReaderWithPosition* inPDFStream,
														const PDFPageRange& inPageRange,
														const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	// MergePDFPagesToPage, merge PDF pages content to an input page. good for single-placement of a page content, cheaper than creating
	// and XObject and later placing, when the intention is to use this graphic just once.
	EPDFStatusCode MergePDFPagesToPage(PDFPage* inPage,
									const string& inPDFFilePath,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());

	EPDFStatusCode MergePDFPagesToPage(PDFPage* inPage,
									IByteReaderWithPosition* inPDFStream,
									const PDFPageRange& inPageRange,
									const ObjectIDTypeList& inCopyAdditionalObjects = ObjectIDTypeList());


	// Copying context, allowing for a continous flow of copying from multiple sources PDFs (create one per source) to target PDF
	PDFDocumentCopyingContext* CreatePDFCopyingContext(const string& inPDFFilePath);

	PDFDocumentCopyingContext* CreatePDFCopyingContext(IByteReaderWithPosition* inPDFStream);


	// fonts [text]
	PDFUsedFont* GetFontForFile(const string& inFontFilePath);
	// second overload is for type 1, when an additional metrics file is available
	PDFUsedFont* GetFontForFile(const string& inFontFilePath,const string& inAdditionalMeticsFilePath);

	// URL links
	// URL should be encoded to be a valid URL, ain't gonna be checking that!
	EPDFStatusCode AttachURLLinktoCurrentPage(const string& inURL,const PDFRectangle& inLinkClickArea);

	// Extensibility, reaching to lower levels
	DocumentsContext& GetDocumentsContext();
	ObjectsContext& GetObjectsContext();
	OutputFile& GetOutputFile();
private:

	ObjectsContext mObjectsContext;
	DocumentsContext mDocumentsContext;

	// for output file workflow, this will be the valid output [stream workflow does not have a file]
	OutputFile mOutputFile;

	void SetupLog(const LogConfiguration& inLogConfiguration);
	void SetupObjectsContext(const PDFCreationSettings& inPDFCreationSettings);
	void ReleaseLog();
	EPDFStatusCode SetupState(const string& inStateFilePath);


};

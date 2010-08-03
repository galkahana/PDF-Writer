#include "JPEGImageHandler.h"
#include "InputFile.h"
#include "JPEGImageParser.h"
#include "Trace.h"
#include "EStatusCode.h"
#include "PDFImageXObject.h"
#include "PDFStream.h"
#include "OutputStreamTraits.h"
#include "JPEGImageHandler.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "IDocumentContextExtender.h"
#include "ProcsetResourcesConstants.h"
#include "DocumentContext.h"
#include "XObjectContentContext.h"
#include "PDFFormXObject.h"

JPEGImageHandler::JPEGImageHandler(void)
{
	mObjectsContext = NULL;
	mDocumentContext = NULL;
	mExtender = NULL;
}

JPEGImageHandler::~JPEGImageHandler(void)
{
}


void JPEGImageHandler::SetOperationsContexts(DocumentContext* inDocumentContext,ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
	mDocumentContext = inDocumentContext;
}

PDFImageXObject* JPEGImageHandler::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inImageXObjectID)
{
	PDFImageXObject* imageXObject = NULL;

	do 
	{
		// retrieve image information
		BoolAndJPEGImageInformation imageInformationResult = RetrieveImageInformation(inJPGFilePath);
		if(!imageInformationResult.first)
		{
			TRACE_LOG1("JPEGImageHandler::CreateImageXObjectJPGFile, unable to retrieve image information for %s",inJPGFilePath.c_str());
			break;
		}

		// Write Image XObject
		imageXObject = CreateAndWriteImageXObjectFromJPGInformation(inJPGFilePath,inImageXObjectID,imageInformationResult.second);

	} while(false);

	return imageXObject;  	

}

static const string scType = "Type";
static const string scXObject = "XObject";
static const string scSubType = "Subtype";

static const string scImage = "Image";
static const string scWidth = "Width";
static const string scHeight = "Height";
static const string scColorSpace = "ColorSpace";
static const string scDeviceGray = "DeviceGray";
static const string scDeviceRGB = "DeviceRGB";
static const string scDeviceCMYK = "DeviceCMYK";
static const string scDecode = "Decode";
static const string scBitsPerComponent = "BitsPerComponent";
static const string scFilter = "Filter";
static const string scDCTDecode = "DCTDecode";
PDFImageXObject* JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation(const wstring& inJPGFilePath,
																				ObjectIDType inImageXObjectID,
																				const JPEGImageInformation& inJPGImageInformation)
{
	PDFImageXObject* imageXObject = NULL;
	EStatusCode status = eSuccess;

	do
	{
		if(!mObjectsContext)
		{
			TRACE_LOG("JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unexpected Error, mObjectsContext not initialized with an objects context");
			break;
		}

		mObjectsContext->StartNewIndirectObject(inImageXObjectID);
		DictionaryContext* imageContext = mObjectsContext->StartDictionary();
	
		// type
		imageContext->WriteKey(scType);
		imageContext->WriteNameValue(scXObject);

		// subtype
		imageContext->WriteKey(scSubType);
		imageContext->WriteNameValue(scImage);

		// Width
		imageContext->WriteKey(scWidth);
		imageContext->WriteIntegerValue(inJPGImageInformation.SamplesWidth);

		// Height
		imageContext->WriteKey(scHeight);
		imageContext->WriteIntegerValue(inJPGImageInformation.SamplesHeight);

		// Bits Per Component
		imageContext->WriteKey(scBitsPerComponent);
		imageContext->WriteIntegerValue(8);

		// Color Space and Decode Array if necessary
		imageContext->WriteKey(scColorSpace);
		switch(inJPGImageInformation.ColorComponentsCount)
		{
		case 1:
				imageContext->WriteNameValue(scDeviceGray);
				break;
		case 3:
				imageContext->WriteNameValue(scDeviceRGB);
				break;
		case 4:
				imageContext->WriteNameValue(scDeviceCMYK);
				
				// Decode array
				imageContext->WriteKey(scDecode);
				mObjectsContext->StartArray();
				for(int i=0;i<4;++i)
				{
					mObjectsContext->WriteDouble(1);
					mObjectsContext->WriteDouble(0);
				}
				mObjectsContext->EndArray();
				mObjectsContext->EndLine();
				break;

		default:
			TRACE_LOG1("JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation, Unexpected Error, unfamilar color components count - %d",
				inJPGImageInformation.ColorComponentsCount);
			status = eFailure;
			break;
		}
		if(status != eSuccess)
			break;

		// Decoder - DCTDecode
		imageContext->WriteKey(scFilter);
		imageContext->WriteNameValue(scDCTDecode);

		if(mExtender)
		{
			if(mExtender->OnJPEGImageXObjectWrite(inImageXObjectID,imageContext,mObjectsContext,mDocumentContext,this) != eSuccess)
			{
				TRACE_LOG("JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation, unexpected faiulre. extender declared failure when writing image xobject.");
				break;
			}
		}	
		
		InputFile JPGFile;
		status = JPGFile.OpenFile(inJPGFilePath);
		if(status != eSuccess)
		{
			TRACE_LOG1("JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unable to open JPG file for reading, %s", inJPGFilePath.c_str());
			break;
		}


		PDFStream* imageStream = mObjectsContext->StartUnfilteredPDFStream(imageContext);

		OutputStreamTraits outputTraits(imageStream->GetWriteStream());
		status = outputTraits.CopyToOutputStream(JPGFile.GetInputStream());
		JPGFile.CloseFile(); // close file regardless
		if(status != eSuccess)
		{
			TRACE_LOG("JPEGImageHandler::CreateAndWriteImageXObjectFromJPGInformation. Unexpected Error, failed to copy jpg stream to output stream");
			delete imageStream;
			break;
		}
	
		mObjectsContext->EndPDFStream(imageStream);
		delete imageStream;

		imageXObject = new PDFImageXObject(inImageXObjectID,1 == inJPGImageInformation.ColorComponentsCount ? KProcsetImageB:KProcsetImageC);
	}while(false);
	

	return imageXObject;
}


BoolAndJPEGImageInformation JPEGImageHandler::RetrieveImageInformation(const wstring& inJPGFilePath)
{
	BoolAndJPEGImageInformation imageInformationResult(false,mNullInformation);

	WStringToJPEGImageInformationMap::iterator it = mImagesInformationMap.find(inJPGFilePath);
	if(it == mImagesInformationMap.end())
	{
		do
		{
			InputFile JPGFile;
			EStatusCode status = JPGFile.OpenFile(inJPGFilePath);
			if(status != eSuccess)
			{
				TRACE_LOG1("JPEGImageHandler::JPEGImageHandler. Unable to open JPG file for reading, %s", inJPGFilePath.c_str());
				break;
			}

			JPEGImageParser jpgImageParser;
			JPEGImageInformation imageInformation;
			
			status = jpgImageParser.Parse(JPGFile.GetInputStream(),imageInformation);
			if(status != eSuccess)
			{
				TRACE_LOG1("JPEGImageHandler::JPEGImageHandler. Failed to parse JPG file, %s", inJPGFilePath.c_str());
				break;
			}

			status = JPGFile.CloseFile();
			if(status != eSuccess)
			{
				TRACE_LOG1("JPEGImageHandler::JPEGImageHandler. Failed to close JPG file, %s", inJPGFilePath.c_str());
				break;
			}

			mImagesInformationMap.insert(WStringToJPEGImageInformationMap::value_type(inJPGFilePath,imageInformation));
			imageInformationResult.first = true;
			imageInformationResult.second = imageInformation;
		} while(false);
	}
	else
	{
		imageInformationResult.first = true;
		imageInformationResult.second = it->second;
	}

	return imageInformationResult;
}

void JPEGImageHandler::SetDocumentContextExtender(IDocumentContextExtender* inExtender)
{
	mExtender = inExtender;
}

PDFFormXObject* JPEGImageHandler::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath,ObjectIDType inFormXObjectID)
{
	PDFImageXObject* imageXObject = NULL;
	PDFFormXObject* imageFormXObject = NULL;

	do 
	{
		if(!mObjectsContext)
		{
			TRACE_LOG("JPEGImageHandler::CreateFormXObjectFromJPGFile. Unexpected Error, mDocumentContex not initialized with a document context");
			break;
		}

		// retrieve image information
		BoolAndJPEGImageInformation imageInformationResult = RetrieveImageInformation(inJPGFilePath);
		if(!imageInformationResult.first)
		{
			TRACE_LOG1("JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to retrieve image information for %s",inJPGFilePath.c_str());
			break;
		}

		// Write Image XObject
		imageXObject = CreateAndWriteImageXObjectFromJPGInformation(inJPGFilePath,mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(),imageInformationResult.second);
		if(!imageXObject)
		{
			TRACE_LOG1("JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to create image xobject for %s",inJPGFilePath.c_str());
			break;
		}

		// Write Image form XObject
		imageFormXObject = CreateImageFormXObjectFromImageXObject(imageXObject,inFormXObjectID,imageInformationResult.second);
		if(!imageFormXObject)
		{
			TRACE_LOG1("JPEGImageHandler::CreateFormXObjectFromJPGFile, unable to create form xobject for %s",inJPGFilePath.c_str());
			break;
		}


	} while(false);

	delete imageXObject;
	return imageFormXObject;  	
}

PDFFormXObject* JPEGImageHandler::CreateImageFormXObjectFromImageXObject(PDFImageXObject* inImageXObject,ObjectIDType inFormXObjectID, const JPEGImageInformation& inJPGImageInformation)
{
	PDFFormXObject* formXObject = NULL;
	do
	{
		if(!mObjectsContext)
		{
			TRACE_LOG("JPEGImageHandler::CreateImageFormXObjectFromImageXObject. Unexpected Error, mDocumentContex not initialized with a document context");
			break;
		}

		DoubleAndDoublePair dimensions = GetImageDimensions(inJPGImageInformation);

		formXObject = mDocumentContext->StartFormXObject(PDFRectangle(0,0,dimensions.first,dimensions.second),inFormXObjectID);
		XObjectContentContext* xobjectContentContext = formXObject->GetContentContext();

		xobjectContentContext->q();
		xobjectContentContext->cm(dimensions.first,0,0,dimensions.second,0,0);
		xobjectContentContext->Do(formXObject->GetResourcesDictionary().AddImageXObjectMapping(inImageXObject));
		xobjectContentContext->Q();

		EStatusCode status = mDocumentContext->EndFormXObjectNoRelease(formXObject);
		if(status != eSuccess)
		{
			TRACE_LOG("JPEGImageHandler::CreateImageFormXObjectFromImageXObject. Unexpected Error, could not create form XObject for image");
			delete formXObject;
			formXObject = NULL;
			break;
		}	


	}while(false);
	return formXObject;
}

DoubleAndDoublePair JPEGImageHandler::GetImageDimensions(const JPEGImageInformation& inJPGImageInformation)
{
	DoubleAndDoublePair returnResult(1,1);

	do
	{
		// prefer JFIF determined resolution
		if(inJPGImageInformation.JFIFInformationExists)
		{
			double jfifXDensity = 	(0 == inJPGImageInformation.JFIFXDensity) ? 1:inJPGImageInformation.JFIFXDensity;
			double jfifYDensity = (0 == inJPGImageInformation.JFIFYDensity) ? 1:inJPGImageInformation.JFIFYDensity;

			switch(inJPGImageInformation.JFIFUnit)
			{
				case 1: // INCH
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0;
					break;
				case 2: // CM
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0 / 2.54;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0 / 2.54;
					break;
				default: // 0 - aspect ratio
					returnResult.first = (double)inJPGImageInformation.SamplesWidth;
					returnResult.second = (double)inJPGImageInformation.SamplesHeight;
					break;
			}
			break;
		}

		// if no jfif try exif
		if(inJPGImageInformation.ExifInformationExists)
		{
			double exifXDensity = 	(0 == inJPGImageInformation.ExifXDensity) ? 1:inJPGImageInformation.ExifXDensity;
			double exifYDensity = (0 == inJPGImageInformation.ExifYDensity) ? 1:inJPGImageInformation.ExifYDensity;

			switch(inJPGImageInformation.ExifUnit)
			{
				case 1: // aspect ratio
					returnResult.first = (double)inJPGImageInformation.SamplesWidth;
					returnResult.second = (double)inJPGImageInformation.SamplesHeight;
					break;
				case 3: // CM
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0 / 2.54;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0 / 2.54;
					break;
				default: // 2 - Inch
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0;
					break;
			}
			break;
		}

		// if no jfif, try photoshop
		if(inJPGImageInformation.PhotoshopInformationExists)
		{
			double photoshopXDensity = 	(0 == inJPGImageInformation.PhotoshopXDensity) ? 1:inJPGImageInformation.PhotoshopXDensity;
			double photoshopYDensity = (0 == inJPGImageInformation.PhotoshopYDensity) ? 1:inJPGImageInformation.PhotoshopYDensity;

			returnResult.first = ((double)inJPGImageInformation.SamplesWidth / photoshopXDensity) * 72.0;
			returnResult.second = ((double)inJPGImageInformation.SamplesHeight / photoshopYDensity) * 72.0;
			break;
		}

		// else aspect ratio
		returnResult.first = (double)inJPGImageInformation.SamplesWidth;
		returnResult.second = (double)inJPGImageInformation.SamplesHeight;
	}while(false);

	return returnResult;
}

PDFImageXObject* JPEGImageHandler::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("JPEGImageHandler::CreateImageXObjectFromJPGFile. Unexpected Error, mObjectsContext not initialized with an objects context");
		return NULL;
	}

	return CreateImageXObjectFromJPGFile(inJPGFilePath,mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());
}

PDFFormXObject* JPEGImageHandler::CreateFormXObjectFromJPGFile(const wstring& inJPGFilePath)
{
	if(!mObjectsContext)
	{
		TRACE_LOG("JPEGImageHandler::CreateFormXObjectFromJPGFile. Unexpected Error, mObjectsContext not initialized with an objects context");
		return NULL;
	}

	return CreateFormXObjectFromJPGFile(inJPGFilePath,mObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID());

}

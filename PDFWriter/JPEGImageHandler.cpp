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

JPEGImageHandler::JPEGImageHandler(void)
{
	mObjectsContext = NULL;
	mExtender = NULL;
}

JPEGImageHandler::~JPEGImageHandler(void)
{
}


void JPEGImageHandler::SetObjectsContext(ObjectsContext* inObjectsContext)
{
	mObjectsContext = inObjectsContext;
}

PDFImageXObject* JPEGImageHandler::CreateImageXObjectFromJPGFile(const wstring& inJPGFilePath)
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
		imageXObject = CreateAndWriteImageXObjectFromJPGInformation(inJPGFilePath,imageInformationResult.second);

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

		ObjectIDType imageXObjectID = mObjectsContext->StartNewIndirectObject();
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
			if(mExtender->OnJPEGImageXObjectWrite(imageXObjectID,imageContext,mObjectsContext,this) != eSuccess)
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

		imageXObject = new PDFImageXObject(imageXObjectID,inJPGImageInformation);
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
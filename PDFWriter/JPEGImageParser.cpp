/*
   Source File : JPEGImageParser.cpp


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
#include "JPEGImageParser.h"
#include "JPEGImageInformation.h"
#include "IByteReaderWithPosition.h"
#include "Trace.h"

#include <memory.h>

using namespace PDFHummus;

JPEGImageParser::JPEGImageParser(void)
{
}

JPEGImageParser::~JPEGImageParser(void)
{
}


const unsigned int scSOF0TagID = 0xc0; //baseline format
const unsigned int scSOF1TagID = 0xc1;
const unsigned int scSOF2TagID = 0xc2;
const unsigned int scSOF3TagID = 0xc3;
const unsigned int scSOF5TagID = 0xc5;
const unsigned int scSOF6TagID = 0xc6;
const unsigned int scSOF7TagID = 0xc7;
const unsigned int scSOF9TagID = 0xc9;
const unsigned int scSOF10TagID = 0xca;
const unsigned int scSOF11TagID = 0xcb;
const unsigned int scSOF13TagID = 0xcd;
const unsigned int scSOF14TagID = 0xce;
const unsigned int scSOF15TagID = 0xcf;
const unsigned int scAPP0TagID = 0xe0; //JFIF marker
const unsigned int scAPP1TagID = 0xe1;	//Exif marker
const unsigned int scAPP13TagID = 0xed;//Photoshop marker
const unsigned int scTagID = 0xff;
const unsigned char scJPEGID[2] = {0xff,0xd8};
const unsigned char scAPP1ID_1[6] = {0x45,0x78,0x69,0x66,0x00,0x00};
const unsigned char scAPP1ID_2[6] = {0x45,0x78,0x69,0x66,0x00,0xFF};
const unsigned char scEOS = '\0';
const unsigned char sc8Bim[4] = {'8','B','I','M'};
const unsigned char scResolutionBIMID[2] = {0x03,0xed};
const unsigned int scAPP1BigEndian = 0x4d4d;
const unsigned int scAPP1LittleEndian = 0x4949;
const unsigned int scAPP1xResolutionTagID = 0x011a;
const unsigned int scAPP1yResolutionTagID = 0x011b;
const unsigned int scAPP1ResolutionUnitTagID = 0x0128;

EStatusCode JPEGImageParser::Parse(IByteReaderWithPosition* inImageStream,JPEGImageInformation& outImageInformation)
{
	EStatusCode status = PDFHummus::eFailure;
	unsigned int tagID;
	bool PhotoshopMarkerNotFound = true;
	bool JFIFMarkerNotFound = true;
	bool SOFMarkerNotFound = true;
	bool ExifMarkerNotFound = true;

	mImageStream = inImageStream;

	do
	{
		status = ReadJPEGID();
		if(status != PDFHummus::eSuccess)
			break;

		do
		{
			if(!mImageStream->NotEnded())
				break;
			status = ReadJpegTag(tagID);
			if(status != PDFHummus::eSuccess)
				break;
			switch(tagID)
			{
				case scSOF0TagID : case scSOF1TagID : case scSOF2TagID:
				case scSOF3TagID : case scSOF5TagID : case scSOF6TagID:
				case scSOF7TagID : case scSOF9TagID : case scSOF10TagID:
				case scSOF11TagID : case scSOF13TagID : case scSOF14TagID:
				case scSOF15TagID :
					status = ReadSOF0Data(outImageInformation);
					if (status == PDFHummus::eSuccess)
						SOFMarkerNotFound = false;
					break;
				case scAPP0TagID:
					if(JFIFMarkerNotFound)
					{
						JFIFMarkerNotFound = false;
						status = ReadJFIFData(outImageInformation);
					}
					else
						SkipTag();
					break;
				case scAPP13TagID:
                	if(PhotoshopMarkerNotFound)
					{
						// photoshop tags may be corrupt, so internal method will return if the 
						// photoshop tag is OK. otherwise skip it, and wait for the next one...parhaps will be better
						bool photoshopDataOK = false;
						status = ReadPhotoshopData(outImageInformation,photoshopDataOK);
						if(PDFHummus::eSuccess == status && photoshopDataOK)
							PhotoshopMarkerNotFound = false;
					}
					else
						status = SkipTag();
					break;				
				case scAPP1TagID:
					if(ExifMarkerNotFound)
					{
						ExifMarkerNotFound = false;
						status = ReadExifData(outImageInformation);
                        if(status != eSuccess)
                        {
                            // if unable to read marker it's either XMP or unsupported version of Exif. simply ignore
                            ExifMarkerNotFound = true;
                            status = eSuccess;
                        }
					}
					else
						status = SkipTag();
					break;
				default:
					status = SkipTag();
					break;
			}
		}
		while((ExifMarkerNotFound || PhotoshopMarkerNotFound || JFIFMarkerNotFound || SOFMarkerNotFound) && (0 == status));

		if (SOFMarkerNotFound)
		{
			status = PDFHummus::eFailure;
			break;
		}
		else
			status = PDFHummus::eSuccess; 
	} 
	while(false);

	return status;
}

EStatusCode JPEGImageParser::ReadJPEGID()
{
	EStatusCode status = ReadStreamToBuffer(2);
	
	if (status != PDFHummus::eSuccess)
		return status;
	
	if (memcmp(mReadBuffer, scJPEGID, 2) != 0)
		return PDFHummus::eFailure;
	
	return PDFHummus::eSuccess;
}

EStatusCode JPEGImageParser::ReadStreamToBuffer(unsigned long inAmountToRead)
{
	if(inAmountToRead == mImageStream->Read(mReadBuffer,inAmountToRead))
		return PDFHummus::eSuccess;
	else
		return PDFHummus::eFailure;
}

EStatusCode JPEGImageParser::ReadJpegTag(unsigned int& outTagID)
{
	EStatusCode status = ReadStreamToBuffer(2);

	if(PDFHummus::eSuccess == status)
	{
		if(scTagID == (unsigned int)mReadBuffer[0])
			outTagID = (unsigned int)mReadBuffer[1];
		else 
			status = PDFHummus::eFailure;
	}
	return status;
}

EStatusCode JPEGImageParser::ReadSOF0Data(JPEGImageInformation& outImageInformation)
{
	unsigned int markerLen;
	EStatusCode status;

	status = ReadStreamToBuffer(8);
	if(PDFHummus::eSuccess == status)
	{
		markerLen = GetIntValue(mReadBuffer);
		if (markerLen < 8)
		{
			TRACE_LOG1("JPEGImageParser::ReadSOF0Data, SOF marker length %u below the 8-byte fixed header", markerLen);
			return PDFHummus::eFailure;
		}
		outImageInformation.SamplesHeight = GetIntValue(mReadBuffer + 3);
		outImageInformation.SamplesWidth = GetIntValue(mReadBuffer + 5);
		outImageInformation.ColorComponentsCount = (unsigned int)mReadBuffer[7];
		SkipStream(markerLen - 8);
	}
	return status;
}

unsigned int JPEGImageParser::GetIntValue(
							const IOBasicTypes::Byte* inBuffer,
							bool inUseLittleEndian)
{
	unsigned int value;

	if (inUseLittleEndian)
	{
		value = (unsigned int)inBuffer[0];
		value += 0x100 * (unsigned int)inBuffer[1];
	}
	else
	{
		value = (unsigned int)inBuffer[1];
		value += 0x100 * (unsigned int)inBuffer[0];
	}

	return value;
}

void JPEGImageParser::SkipStream(unsigned long inSkip)
{
	mImageStream->Skip(inSkip);
}


EStatusCode JPEGImageParser::ReadJFIFData(JPEGImageInformation& outImageInformation)
{
	unsigned int markerLen;
	EStatusCode status;

	status = ReadStreamToBuffer(14);
	if(PDFHummus::eSuccess == status)
	{
		markerLen = GetIntValue(mReadBuffer);
		if (markerLen < 14)
		{
			TRACE_LOG1("JPEGImageParser::ReadJFIFData, JFIF marker length %u below the 14-byte fixed header", markerLen);
			return PDFHummus::eFailure;
		}
		outImageInformation.JFIFInformationExists = true;
		outImageInformation.JFIFUnit = (unsigned int)mReadBuffer[9];
		outImageInformation.JFIFXDensity = GetIntValue(mReadBuffer + 10);
		outImageInformation.JFIFYDensity = GetIntValue(mReadBuffer + 12);
		SkipStream(markerLen - 14);
	}
	return status;
}

TwoLevelStatus JPEGImageParser::ReadStreamToBuffer(unsigned long inAmountToRead,unsigned long& refReadLimit)
{
	if (refReadLimit < inAmountToRead)
		return TwoLevelStatus(PDFHummus::eSuccess, PDFHummus::eFailure);
	EStatusCode status = ReadStreamToBuffer(inAmountToRead);
	if (status == PDFHummus::eSuccess)
		refReadLimit -= inAmountToRead;
	return TwoLevelStatus(status, PDFHummus::eSuccess);
}
TwoLevelStatus JPEGImageParser::ReadLongValue(
	unsigned long& refReadLimit,
	unsigned long& outLongValue,
	bool inUseLittleEndian)
{
	if (refReadLimit < 4)
		return TwoLevelStatus(PDFHummus::eSuccess, PDFHummus::eFailure);

	EStatusCode status = ReadLongValue(outLongValue, inUseLittleEndian);
	if (status == PDFHummus::eSuccess)
		refReadLimit -= 4;
	return TwoLevelStatus(status, PDFHummus::eSuccess);
}

TwoLevelStatus JPEGImageParser::ReadIntValue(
	unsigned long& refReadLimit,
	unsigned int& outIntValue,
	bool inUseLittleEndian)
{
	if (refReadLimit < 2)
		return TwoLevelStatus(PDFHummus::eSuccess, PDFHummus::eFailure);

	EStatusCode status = ReadIntValue(outIntValue, inUseLittleEndian);
	if (status == PDFHummus::eSuccess)
		refReadLimit -= 2;
	return TwoLevelStatus(status, PDFHummus::eSuccess);
}

EStatusCode JPEGImageParser::SkipStream(unsigned long inSkip, unsigned long& refReadLimit)
{
	if (refReadLimit < inSkip)
		return PDFHummus::eFailure;
	SkipStream(inSkip);
	refReadLimit -= inSkip;
	return PDFHummus::eSuccess;
}

EStatusCode JPEGImageParser::ReadPhotoshopData(JPEGImageInformation& outImageInformation, bool& outPhotoshopDataOK)
{
	// code below uses a two level status where the primary is in charge of read error
	// and the secondary is in charge of realizing whether the data is correct. 
	// error in the former should cause complete break. error in the latter is fine on the read level
	// and simply means the data is logically corrupt and should simply be skipped

	TwoLevelStatus twoLevelStatus(eSuccess,eSuccess);
	unsigned int intSkip;
	unsigned long toSkip;
	unsigned int nameSkip;
	unsigned long dataLength;
	bool resolutionBimNotFound = true;

	do {
		twoLevelStatus.primary = ReadIntValue(intSkip);
		if (twoLevelStatus.primary != PDFHummus::eSuccess)
			break;
		if (intSkip < 2)
		{
			TRACE_LOG1("JPEGImageParser::ReadPhotoshopData, Photoshop marker length %u below the 2-byte length field", intSkip);
			twoLevelStatus.primary = PDFHummus::eFailure;
			break;
		}
		toSkip = intSkip - 2;
		twoLevelStatus.primary = SkipTillChar(scEOS, toSkip);
		if (twoLevelStatus.primary != PDFHummus::eSuccess)
			break;

		while (toSkip > 0 && resolutionBimNotFound)
		{
			twoLevelStatus = ReadStreamToBuffer(4, toSkip);
			if(twoLevelStatus.eitherBad())
				break;
			if (0 != memcmp(mReadBuffer, sc8Bim, 4))
				break; 
			twoLevelStatus = ReadStreamToBuffer(3,toSkip);
			if (twoLevelStatus.eitherBad())
				break;
			nameSkip = (int)mReadBuffer[2];
			if (nameSkip % 2 == 0)
				++nameSkip;
			twoLevelStatus.secondary = SkipStream(nameSkip, toSkip);
			if (twoLevelStatus.secondary)
				break;
			resolutionBimNotFound = (0 != memcmp(mReadBuffer, scResolutionBIMID, 2));
			twoLevelStatus = ReadLongValue(toSkip, dataLength);
			if (twoLevelStatus.eitherBad())
				break;
			if (resolutionBimNotFound)
			{
				if (dataLength % 2 == 1)
					++dataLength;
				twoLevelStatus.secondary = SkipStream(dataLength, toSkip);
				if (twoLevelStatus.secondary != PDFHummus::eSuccess)
					break;
			}
			else
			{
				twoLevelStatus = ReadStreamToBuffer(16, toSkip);
				if (twoLevelStatus.eitherBad())
					break;

				outImageInformation.PhotoshopInformationExists = true;
				outImageInformation.PhotoshopXDensity = GetIntValue(mReadBuffer) + GetFractValue(mReadBuffer + 2);
				outImageInformation.PhotoshopYDensity = GetIntValue(mReadBuffer + 8) + GetFractValue(mReadBuffer + 10);
			}
		}

		if (PDFHummus::eSuccess == twoLevelStatus.primary)
			SkipStream(toSkip);
	} while (false);

	outPhotoshopDataOK = !resolutionBimNotFound && twoLevelStatus.secondary == PDFHummus::eSuccess;
	return twoLevelStatus.primary;
}

EStatusCode JPEGImageParser::ReadExifData(JPEGImageInformation& outImageInformation)
{
	EStatusCode status = PDFHummus::eSuccess;
	TwoLevelStatus tls(PDFHummus::eSuccess, PDFHummus::eSuccess);
	unsigned long toSkip = 0;
	unsigned int markerLen;
	unsigned long ifdOffset = 0;
	unsigned int ifdDirectorySize;
	unsigned int tagID;
	unsigned int encodingType;
	bool isBigEndian = false;
	unsigned long xResolutionOffset = 0;
	unsigned long yResolutionOffset = 0;
	unsigned int resolutionUnitValue = 0;

	do
	{
		// read Exif Tag size
		status = ReadIntValue(markerLen);
		if (status != PDFHummus::eSuccess)
			break;
		if (markerLen < 2)
		{
			TRACE_LOG1("JPEGImageParser::ReadExifData, APP1 marker length %u below the 2-byte length field", markerLen);
			status = PDFHummus::eFailure;
			break;
		}
		toSkip = (unsigned long)(markerLen - 2);

		// read Exif ID (6 bytes "Exif\0\0" or "Exif\0\xff")
		tls = ReadStreamToBuffer(6, toSkip);
		if (tls.primary != PDFHummus::eSuccess)
		{
			status = tls.primary;
			break;
		}
		if (tls.secondary != PDFHummus::eSuccess ||
			(memcmp(mReadBuffer, scAPP1ID_1, 6) != 0 && memcmp(mReadBuffer, scAPP1ID_2, 6) != 0))
		{
			// might be wrong ID (XMP / unsupported)
			TRACE_LOG("JPEGImageParser::ReadExifData, APP1 identifier did not match \"Exif\\0\\0\" or \"Exif\\0\\xff\"");
			status = PDFHummus::eFailure;
			break;
		}

		// read encoding (2 bytes)
		tls = ReadIntValue(toSkip, encodingType);
		if (tls.eitherBad())
		{
			status = PDFHummus::eFailure;
			break;
		}
		if (encodingType == scAPP1BigEndian)
			isBigEndian = true;
		else if (encodingType == scAPP1LittleEndian)
			isBigEndian = false;
		else
		{
			TRACE_LOG1("JPEGImageParser::ReadExifData, TIFF endianness marker 0x%04x is neither MM nor II", encodingType);
			status = PDFHummus::eFailure;
			break;
		}

		// skip 0x002a magic
		status = SkipStream(2, toSkip);
		if (status != PDFHummus::eSuccess)
			break;

		// read IFD0 offset
		tls = ReadLongValue(toSkip, ifdOffset, !isBigEndian);
		if (tls.eitherBad())
		{
			status = PDFHummus::eFailure;
			break;
		}

		// IFD0 offset is relative to the TIFF header start; TIFF header is 8 bytes, so any
		// in-spec offset is >= 8.
		if (ifdOffset < 8)
		{
			TRACE_LOG1("JPEGImageParser::ReadExifData, IFD0 offset %lu below the 8-byte TIFF header minimum", ifdOffset);
			status = PDFHummus::eFailure;
			break;
		}

		// skip to the IFD beginning
		status = SkipStream(ifdOffset - 8, toSkip);
		if (status != PDFHummus::eSuccess)
			break;

		// read IFD size
		tls = ReadIntValue(toSkip, ifdDirectorySize, !isBigEndian);
		if (tls.eitherBad())
		{
			status = PDFHummus::eFailure;
			break;
		}

		for (unsigned int i = 0; i < ifdDirectorySize; i++)
		{
			if (0 != xResolutionOffset && 0 != yResolutionOffset && 0 != resolutionUnitValue)
			{
				status = SkipStream(12UL * (ifdDirectorySize - i), toSkip);
				break;
			}

			tls = ReadIntValue(toSkip, tagID, !isBigEndian);
			if (tls.eitherBad())
			{
				status = PDFHummus::eFailure;
				break;
			}

			switch (tagID)
			{
				case scAPP1xResolutionTagID:
					status = SkipStream(6, toSkip);
					if (status != PDFHummus::eSuccess) break;
					tls = ReadLongValue(toSkip, xResolutionOffset, !isBigEndian);
					if (tls.eitherBad()) status = PDFHummus::eFailure;
					break;
				case scAPP1yResolutionTagID:
					status = SkipStream(6, toSkip);
					if (status != PDFHummus::eSuccess) break;
					tls = ReadLongValue(toSkip, yResolutionOffset, !isBigEndian);
					if (tls.eitherBad()) status = PDFHummus::eFailure;
					break;
				case scAPP1ResolutionUnitTagID:
					status = SkipStream(6, toSkip);
					if (status != PDFHummus::eSuccess) break;
					tls = ReadIntValue(toSkip, resolutionUnitValue, !isBigEndian);
					if (tls.eitherBad()) { status = PDFHummus::eFailure; break; }
					status = SkipStream(2, toSkip);
					break;
				default:
					status = SkipStream(10, toSkip);
					break;
			}

			if (status != PDFHummus::eSuccess)
				break;
		}
		if (status != PDFHummus::eSuccess)
			break;

		outImageInformation.ExifInformationExists = true;
		if (resolutionUnitValue != 0)
			outImageInformation.ExifUnit = resolutionUnitValue;
		else
			outImageInformation.ExifUnit = 2;

		unsigned long currentOffset = 0;
		unsigned long rewindAmount = ifdOffset + (unsigned long)ifdDirectorySize * 12 + 2;
		if (ifdOffset > 8)
		{
			// the IFD data may appear before the IFD header; rewind to the TIFF header start so
			// in-Exif offsets can be reached by forward seek
			LongFilePositionType currentPos = mImageStream->GetCurrentPosition();
			if (currentPos < 0 || (unsigned long long)currentPos < (unsigned long long)rewindAmount)
			{
				TRACE_LOG2("JPEGImageParser::ReadExifData, TIFF-header rewind %lu exceeds current stream position %lld", rewindAmount, (long long)currentPos);
				status = PDFHummus::eFailure;
				break;
			}
			mImageStream->SetPosition(currentPos - (LongFilePositionType)rewindAmount);
			toSkip += rewindAmount;
		}
		else
		{
			currentOffset = rewindAmount;
		}
		unsigned long tempOffset = currentOffset;
		// Measure actual stream consumption rather than relying on
		// tempOffset: GetResolutionFromExif's inoutOffset only advances on
		// success, so a partial read leaves it stale.
		LongFilePositionType posBefore = mImageStream->GetCurrentPosition();
		EStatusCode getResStatus = GetResolutionFromExif(outImageInformation, xResolutionOffset, yResolutionOffset, tempOffset, !isBigEndian);
		LongFilePositionType posAfter = mImageStream->GetCurrentPosition();
		unsigned long consumed = (posAfter > posBefore) ? (unsigned long)(posAfter - posBefore) : 0;
		if (consumed > toSkip)
		{
			TRACE_LOG2("JPEGImageParser::ReadExifData, resolution read consumed %lu bytes, exceeds remaining marker budget %lu", consumed, toSkip);
			status = PDFHummus::eFailure;
			toSkip = 0;
			break;
		}
		toSkip -= consumed;
		if (getResStatus != PDFHummus::eSuccess)
		{
			status = getResStatus;
			break;
		}

		SkipStream(toSkip);
		toSkip = 0;
	}
	while(false);

	// Drain any remaining marker bytes so Parse can locate the next marker
	// after a partial Exif rejection. Parse treats Exif failure as "not Exif,
	// try the next APP1" and re-enters the loop.
	if (toSkip > 0)
		SkipStream(toSkip);

	return status;
}

EStatusCode JPEGImageParser::GetResolutionFromExif(
							   JPEGImageInformation& outImageInformation,
							   unsigned long inXResolutionOffset,
							   unsigned long inYResolutionOffset,
							   unsigned long& inoutOffset,
							   bool inUseLittleEndian)
{	
	unsigned long firstOffset = 0, secondOffset = 0;
	bool xResolutionIsFirst = true;
	EStatusCode status = PDFHummus::eSuccess;

	outImageInformation.ExifXDensity = 0;
	outImageInformation.ExifYDensity = 0;

	if (inXResolutionOffset != 0 && inXResolutionOffset < inYResolutionOffset)
	{
		firstOffset = inXResolutionOffset;

		if (inYResolutionOffset != 0)
			secondOffset = inYResolutionOffset;	
	}
	else if (inYResolutionOffset != 0)
	{
		firstOffset = inYResolutionOffset;
		xResolutionIsFirst = false;

		if (inXResolutionOffset != 0)
			secondOffset = inXResolutionOffset;	
	}

	do
	{
		if (0 == firstOffset)
			break;

		if (firstOffset < inoutOffset)
		{
			TRACE_LOG2("JPEGImageParser::GetResolutionFromExif, first resolution offset %lu precedes parser position %lu", firstOffset, inoutOffset);
			status = PDFHummus::eFailure;
			break;
		}
		SkipStream(firstOffset - inoutOffset);
		inoutOffset = firstOffset;

		status = ReadRationalValue(
			xResolutionIsFirst? outImageInformation.ExifXDensity : outImageInformation.ExifYDensity,
			inUseLittleEndian);

		if (status != PDFHummus::eSuccess)
			break;

		inoutOffset += 8;

		if (0 == secondOffset)
			break;

		if (secondOffset < inoutOffset)
		{
			TRACE_LOG2("JPEGImageParser::GetResolutionFromExif, second resolution offset %lu precedes parser position %lu", secondOffset, inoutOffset);
			status = PDFHummus::eFailure;
			break;
		}
		SkipStream(secondOffset - inoutOffset);
		inoutOffset = secondOffset;

		status = ReadRationalValue(
			xResolutionIsFirst? outImageInformation.ExifYDensity : outImageInformation.ExifXDensity,
			inUseLittleEndian);
		if (status != PDFHummus::eSuccess)
			break;

		inoutOffset += 8;
	} while(false);
	return status;
}

EStatusCode JPEGImageParser::ReadRationalValue(
							   double& outDoubleValue,
							   bool inUseLittleEndian)
{
	unsigned long numerator, denominator;
	EStatusCode status = ReadLongValue(numerator, inUseLittleEndian);

	if (status != PDFHummus::eSuccess)
		return status;

	status = ReadLongValue(denominator, inUseLittleEndian);

	if (status != PDFHummus::eSuccess)
		return status;

	if (denominator == 0)
	{
		TRACE_LOG("JPEGImageParser::ReadRationalValue, rational denominator is zero");
		return PDFHummus::eFailure;
	}

	outDoubleValue = ((double) numerator) / ((double) denominator);
	return status;
}

EStatusCode JPEGImageParser::ReadIntValue(
						unsigned int& outIntValue,
						bool inUseLittleEndian)
{
	EStatusCode status = ReadStreamToBuffer(2);

	if(PDFHummus::eSuccess == status)
		outIntValue = GetIntValue(mReadBuffer, inUseLittleEndian);
	return status;
}

EStatusCode JPEGImageParser::SkipTillChar(IOBasicTypes::Byte inSkipUntilValue,unsigned long& refSkipLimit)
{
	EStatusCode status = PDFHummus::eSuccess;
	bool charNotFound = true;
	
	while(charNotFound && (PDFHummus::eSuccess == status) && (refSkipLimit > 0))
	{
		status = ReadStreamToBuffer(1);
		if(PDFHummus::eSuccess == status)
		{
			--refSkipLimit;
			if(mReadBuffer[0] == inSkipUntilValue)
				charNotFound = false;
		}
	}
	return status;
}

EStatusCode JPEGImageParser::ReadLongValue(	unsigned long& outLongValue,
											bool inUseLittleEndian)
{
	EStatusCode status = ReadStreamToBuffer(4);

	if(PDFHummus::eSuccess == status)
		outLongValue = GetLongValue(mReadBuffer, inUseLittleEndian);
	return status;
}

unsigned long JPEGImageParser::GetLongValue(
							const IOBasicTypes::Byte* inBuffer,
							bool inUseLittleEndian)
{
	unsigned long value;

	if (inUseLittleEndian)
	{
		value = (unsigned int)inBuffer[0];
		value += 0x100 * (unsigned int)inBuffer[1];
		value += 0x10000 * (unsigned int)inBuffer[2];
		value += 0x1000000 * (unsigned int)inBuffer[3];
	}
	else
	{
		value = (unsigned int)inBuffer[3];
		value += 0x100 * (unsigned int)inBuffer[2];
		value += 0x10000 * (unsigned int)inBuffer[1];
		value += 0x1000000 * (unsigned int)inBuffer[0];
	}
	return value;
}

double JPEGImageParser::GetFractValue(const IOBasicTypes::Byte* inBuffer)
{
	double value;

	value = (double)inBuffer[0] / (double)0x100 +
			(double)inBuffer[1] / (double)0x10;
	return value;
}


EStatusCode JPEGImageParser::SkipTag()
{
	EStatusCode status;
	unsigned int toSkip;

	status = ReadIntValue(toSkip);
	// skipping -2 because int was already read
	if(PDFHummus::eSuccess == status)
	{
		if (toSkip < 2)
		{
			TRACE_LOG1("JPEGImageParser::SkipTag, marker length %u below the 2-byte length field", toSkip);
			return PDFHummus::eFailure;
		}
		SkipStream(toSkip-2);
	}
	return status;
}

/*
   Source File : JPEGImageParser.h


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

#include "EStatusCode.h"
#include "IOBasicTypes.h"

struct JPEGImageInformation;
class IByteReaderWithPosition;

class JPEGImageParser
{
public:
	JPEGImageParser(void);
	~JPEGImageParser(void);

	EStatusCode Parse(IByteReaderWithPosition* inImageStream,JPEGImageInformation& outImageInformation);

private:

	IByteReaderWithPosition* mImageStream;
	IOBasicTypes::Byte mReadBuffer[500];

	EStatusCode ReadJPEGID();
	EStatusCode ReadStreamToBuffer(unsigned long inAmountToRead);
	EStatusCode ReadJpegTag(unsigned int& outTagID);
	EStatusCode ReadSOF0Data(JPEGImageInformation& outImageInformation);
	unsigned int GetIntValue(const IOBasicTypes::Byte* inBuffer,
							 bool inUseLittleEndian = false);
	void SkipStream(unsigned long inSkip);
	EStatusCode ReadJFIFData(JPEGImageInformation& outImageInformation);
	EStatusCode ReadPhotoshopData(JPEGImageInformation& outImageInformation);
	EStatusCode ReadExifData(JPEGImageInformation& outImageInformation);
	EStatusCode GetResolutionFromExif(	JPEGImageInformation& outImageInformation,
									   unsigned long inXResolutionOffset,
									   unsigned long inYResolutionOffset,
									   unsigned long& inoutOffset,
									   bool inUseLittleEndian);
	EStatusCode ReadRationalValue(double& outDoubleValue,
								  bool inUseLittleEndian);
	EStatusCode ReadExifID();
	EStatusCode IsBigEndianExif(bool& outIsBigEndian);
	EStatusCode ReadIntValue(	unsigned int& outIntValue,
								bool inUseLittleEndian = false);
	EStatusCode SkipTillChar(IOBasicTypes::Byte inSkipUntilValue,unsigned long& refSkipLimit);
	EStatusCode ReadLongValue(	unsigned long& outLongValue,
								bool inUseLittleEndian = false);
	unsigned long GetLongValue(	const IOBasicTypes::Byte* inBuffer,
								bool inUseLittleEndian);
	double GetFractValue(const IOBasicTypes::Byte* inBuffer);
	EStatusCode SkipTag();
};

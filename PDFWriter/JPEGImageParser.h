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

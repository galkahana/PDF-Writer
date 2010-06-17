#pragma once

struct JPEGImageInformation
{
	JPEGImageInformation(void);

	// basic image information
	long long SamplesWidth;
	long long SamplesHeight;
	int ColorComponentsCount;

	// JFIF Information
	bool JFIFInformationExists;
	unsigned int JFIFUnit;
	double JFIFXDensity;
	double JFIFYDensity;

	// Exif Information
	bool ExifInformationExists;
	unsigned int ExifUnit;
	double ExifXDensity;
	double ExifYDensity;

	// Photoshop Information
	bool PhotoshopInformationExists;
	// photoshop density is always pixel per inch
	double PhotoshopXDensity;
	double PhotoshopYDensity;

};

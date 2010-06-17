#include "JPEGImageInformation.h"

JPEGImageInformation::JPEGImageInformation(void)
{

	SamplesWidth = 0;
	SamplesHeight = 0;
	ColorComponentsCount= 0;

	JFIFInformationExists = false;
	JFIFUnit = 0;
	JFIFXDensity = 0;
	JFIFYDensity = 0;

	ExifInformationExists = false;
	ExifUnit = 0;
	ExifXDensity = 0;
	ExifYDensity = 0;

	PhotoshopInformationExists = false;
	PhotoshopXDensity = 0;
	PhotoshopYDensity = 0;
}



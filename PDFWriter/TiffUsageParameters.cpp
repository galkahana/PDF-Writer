#include "TiffUsageParameters.h"

const TIFFBiLevelBWColorTreatment TIFFBiLevelBWColorTreatment::DefaultTIFFBiLevelBWColorTreatment(false,CMYKRGBColor::CMYKBlack);

const TIFFBiLevelGrayscaleColorTreatment TIFFBiLevelGrayscaleColorTreatment::DefaultTIFFBiLevelGrayscaleColorTreatment(false,CMYKRGBColor::CMYKBlack,CMYKRGBColor::CMYKWhite);

const TIFFUsageParameters TIFFUsageParameters::DefaultTIFFUsageParameters(	0,
																			TIFFBiLevelBWColorTreatment::DefaultTIFFBiLevelBWColorTreatment,				
																			TIFFBiLevelGrayscaleColorTreatment::DefaultTIFFBiLevelGrayscaleColorTreatment);
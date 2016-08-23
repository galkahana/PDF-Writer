/*
 * ICUUnicodeTextUtils.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#ifndef PDFHUMMUS_NO_ICUBIDI

#include "ICUUnicodeTextUtils.h"
#include "unicode/ptypes.h"
#include "unicode/urename.h"
#include "unicode/unistr.h"
#include "unicode/ushape.h"
#include "unicode/ucnv.h"
#include "unicode/ubidi.h"
#include "unicode/ucnv.h"
#include "Trace.h"
#include <iostream>
#include <cstdlib>


using namespace std;

EStatusCode ICUUnicodeTextUtils::getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset) {

	UErrorCode errorCode=U_ZERO_ERROR;
	UConverter *conv = ucnv_open(charset.c_str(), &errorCode);
    int32_t destLength = logicalString.length()+1;
	if (conv == NULL) {
		TRACE_LOG1("ICU Error: %s", u_errorName(errorCode));
    	return eFailure;
    }

    UChar* uSourceText = new UChar[destLength];
    UChar* uReorderedText = new UChar[destLength];
    UChar* uShapedText = new UChar[destLength];


    ucnv_toUChars(conv, uSourceText, destLength,
    		logicalString.c_str(), logicalString.length(), &errorCode);


    if (errorCode != U_ZERO_ERROR) {
    	TRACE_LOG1("ICU Error: %s", u_errorName(errorCode));
    	delete[] uSourceText;
    	delete[] uReorderedText;
    	delete[] uShapedText;
    	ucnv_close(conv);
		return eFailure;
	}



    UBiDi* bidi = ubidi_open();

    ubidi_setPara(bidi, uSourceText, u_strlen(uSourceText), UBIDI_DEFAULT_RTL, NULL, &errorCode);


    ubidi_writeReordered(bidi, uReorderedText, u_strlen(uSourceText)+1, UBIDI_DO_MIRRORING, &errorCode);



    u_shapeArabic(   uReorderedText,
 	destLength,
 	uShapedText,
 	destLength,
     U_SHAPE_LETTERS_SHAPE|U_SHAPE_TEXT_DIRECTION_LOGICAL|U_SHAPE_TEXT_DIRECTION_VISUAL_LTR,
      &errorCode
    );


	int32_t targetCap = UCNV_GET_MAX_BYTES_FOR_STRING(u_strlen(uShapedText),
	                                                    ucnv_getMaxCharSize(conv));

	char *target = (char*)malloc(targetCap);
	ucnv_fromUChars(conv, target, targetCap, uShapedText, u_strlen(uShapedText), &errorCode);

	if (errorCode != U_ZERO_ERROR) {
		TRACE_LOG1("ICU Error: %s", u_errorName(errorCode));
		delete[] uSourceText;
		delete[] uReorderedText;
		delete[] uShapedText;
		ucnv_close(conv);
		free(target);
		ubidi_close(bidi);
		return eFailure;
	}

	outVisualString = target;

	delete[] uSourceText;
	delete[] uReorderedText;
	delete[] uShapedText;
	ucnv_close(conv);
	free(target);
	ubidi_close(bidi);

	return (errorCode == U_ZERO_ERROR) ?eSuccess :eFailure;
}


#endif /* PDFHUMMUS_NO_ICUBIDI */

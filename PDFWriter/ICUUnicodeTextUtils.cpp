/*
 * ICUUnicodeTextUtils.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#include "ICUUnicodeTextUtils.h"
#include "unicode/ptypes.h"
#include "unicode/urename.h"
#include "unicode/unistr.h"
#include "unicode/ushape.h"

EStatusCode ICUUnicodeTextUtils::getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset) {

	UnicodeString srcSring=UNICODE_STRING(logicalString.c_str(), logicalString.length());
	UChar* shapedText = new UChar[srcSring.length()+1];
	UErrorCode errorCode = U_ZERO_ERROR;

	u_shapeArabic   (   srcSring.getBuffer(),
			srcSring.length(),
			shapedText,
			srcSring.length()+1,
			U_SHAPE_LETTERS_UNSHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_BEGINNING|U_SHAPE_TEXT_DIRECTION_LOGICAL,
			&errorCode
	);

	UnicodeString dest_text(shapedText);
//	std::string utf8;
	dest_text.toUTF8String(outVisualString);

	return (errorCode == U_ZERO_ERROR) ?eSuccess :eFailure;
}

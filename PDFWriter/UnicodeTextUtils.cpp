/*
 * UnicodeTextUtils.cpp
 *
 *  Created on: Aug 16, 2016
 *      Author: amr
 */
#ifdef PDFHUMMUS_BIDI
#include <fribidi-char-sets.h>
#include <fribidi.h>
#include <fribidi-common.h>
#include "UnicodeTextUtils.h"
#include "EStatusCode.h"
#include <cstdlib>

using namespace PDFHummus;

UnicodeTextUtils::UnicodeTextUtils() {
	// TODO Auto-generated constructor stub

}

UnicodeTextUtils::~UnicodeTextUtils() {
	// TODO Auto-generated destructor stub
}

//Please note that this code uses malloc and free as they're compatible with the way fribidi works
EStatusCode UnicodeTextUtils::getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& inCharset) {
	std::string visualString;
	int logical_str_len = logicalString.length();
	int  visual_str_len, ustr_len;
	char *visual_str;

	FriBidiCharSet charset = fribidi_parse_charset (inCharset.c_str());
	if(!charset) {
		return eFailure;
	}

	long direction = FRIBIDI_PAR_ON;
	FriBidiLevel   status;
	FriBidiParType base_direction;

	// Convert input string to internal Unicode
	FriBidiChar    *logical_ustr, *visual_ustr;
	logical_ustr = (FriBidiChar*) malloc(sizeof(FriBidiChar) * logical_str_len);
	//ustr_len = fribidi_charset_to_unicode(charset, logical_str, logical_str_len, logical_ustr);
	ustr_len = fribidi_charset_to_unicode(charset, logicalString.c_str(), logical_str_len, logical_ustr);

	// Visualize the Unicode string
	base_direction = direction;
	visual_ustr = (FriBidiChar*) malloc(sizeof(FriBidiChar) * ustr_len);
	status = fribidi_log2vis(logical_ustr, ustr_len, &base_direction, visual_ustr, NULL, NULL, NULL);
	free(logical_ustr);

	// Return false if FriBidi failed
	if (status == 0) {
		free(visual_ustr);
		return eFailure;
	}

	// Convert back from internal Unicode to original character set
	visual_str_len = 4 * ustr_len;	// FriBidi doesn't generate UTF-8 chars longer than 4 bytes
	visual_str = (char *) malloc(sizeof(char) * visual_str_len);
	visual_str_len = fribidi_unicode_to_charset(charset, visual_ustr, ustr_len, visual_str);
	free(visual_ustr);
	outVisualString = visual_str;
	return eSuccess;

}
#endif //PDFHUMMUS_BIDI

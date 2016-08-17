/*
 * UnicodeTextUtils.h
 *
 *  Created on: Aug 16, 2016
 *      Author: amr
 */

#pragma once

//using namespace PDFHummus;
#include <string>
#include "EStatusCode.h"

using namespace PDFHummus;


class UnicodeTextUtils {
public:
	UnicodeTextUtils();
	virtual ~UnicodeTextUtils();
#ifdef PDFHUMMUS_BIDI
	EStatusCode getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset="UTF-8");
#endif //PDFHUMMUS_BIDI
};

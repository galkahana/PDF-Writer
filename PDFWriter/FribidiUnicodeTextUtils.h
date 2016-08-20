/*
 * FribidiUnicodeTextUtils.h
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#ifndef PDFWRITER_FRIBIDIUNICODETEXTUTILS_H_
#define PDFWRITER_FRIBIDIUNICODETEXTUTILS_H_

#include "AbstractUnicodeTextUtils.h"

class FribidiUnicodeTextUtils: public AbstractUnicodeTextUtils {
	virtual EStatusCode getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset="UTF-8");
};

#endif /* PDFWRITER_FRIBIDIUNICODETEXTUTILS_H_ */

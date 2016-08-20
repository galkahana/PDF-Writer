/*
 * ICUUnicodeTextUtils.h
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#ifndef PDFWRITER_ICUUNICODETEXTUTILS_H_
#define PDFWRITER_ICUUNICODETEXTUTILS_H_

#include "AbstractUnicodeTextUtils.h"
#include "ICUUnicodeTextUtils.h"

class ICUUnicodeTextUtils: public AbstractUnicodeTextUtils {
	virtual EStatusCode getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset="UTF-8");
};

#endif /* PDFWRITER_ICUUNICODETEXTUTILS_H_ */

/*
 * UnicodeTextUtilsFactory.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#include "UnicodeTextUtilsFactory.h"
#ifndef PDFHUMMUS_NO_FRIBIDI
#include "FribidiUnicodeTextUtils.h"
#else
#ifndef PDFHUMMUS_NO_ICUBIDI
#include "ICUUnicodeTextUtils.h"
#else
#endif /* PDFHUMMUS_NO_ICUBIDI */
#include "UnicodeTextUtilsNOOP.h"
#endif /* PDFHUMMUS_NO_FRIBIDI */


UnicodeTextUtilsFactory::UnicodeTextUtilsFactory() {}

AbstractUnicodeTextUtils* UnicodeTextUtilsFactory::getUnicodeTextUtils() {
#ifndef PDFHUMMUS_NO_FRIBIDI
	return new FribidiUnicodeTextUtils();
#else
#ifndef PDFHUMMUS_NO_ICUBIDI
	return new ICUUnicodeTextUtils();
#else
	return UnicodeTextUtilsNOOP::getInstance();
#endif /* PDFHUMMUS_NO_ICUBIDI */
#endif /* PDFHUMMUS_NO_FRIBIDI */
}

void UnicodeTextUtilsFactory::deleteInstance(
		AbstractUnicodeTextUtils* instance) {
#ifndef PDFHUMMUS_NO_FRIBIDI
	delete instance;
#else
#ifndef PDFHUMMUS_NO_ICUBIDI
	delete instance;
#else
		//THE NOOP doesn't need to be deleted
#endif /* PDFHUMMUS_NO_ICUBIDI */
#endif /* PDFHUMMUS_NO_FRIBIDI */

}

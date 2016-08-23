/*
 * UnicodeTextUtilsFactory.h
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#include "AbstractUnicodeTextUtils.h"

#ifndef PDFWRITER_UNICODETEXTUTILSFACTORY_H_
#define PDFWRITER_UNICODETEXTUTILSFACTORY_H_

class UnicodeTextUtilsFactory {

	private:
	UnicodeTextUtilsFactory();

	public:
	static AbstractUnicodeTextUtils* getUnicodeTextUtils();
	static void deleteInstance(AbstractUnicodeTextUtils* instance);
};

#endif /* PDFWRITER_UNICODETEXTUTILSFACTORY_H_ */

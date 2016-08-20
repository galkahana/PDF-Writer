/*
 * UnicodeTextUtilsFactory.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: amr
 */

#include "UnicodeTextUtilsFactory.h"
#ifndef PDFHUMMUS_NO_BIDI
#include "FribidiUnicodeTextUtils.h"
#else
#include "ICUUnicodeTextUtils.h"
#endif

#include "ICUUnicodeTextUtils.h"
#include "UnicodeTextUtilsNOOP.h"

#include <iostream>
using std::cout;
using std::endl;

UnicodeTextUtilsFactory::UnicodeTextUtilsFactory() {}

AbstractUnicodeTextUtils* UnicodeTextUtilsFactory::getUnicodeTextUtils() {
#ifndef PDFHUMMUS_NO_BIDI
	cout<<"Returning new ICUUnicodeTextUtils"<<endl;
	return new ICUUnicodeTextUtils();

//	cout<<"Returning new FribidiUnicodeTextUtils"<<endl;
//	return new FribidiUnicodeTextUtils();
#else
	return UnicodeTextUtilsNOOP::getInstance();
#endif
	cout<<"Returning nothing"<<endl;
}

/*
 * UnicodeTextUtilsNOOP.cpp
 *
 *  Created on: Aug 21, 2016
 *      Author: amr
 */

#include "UnicodeTextUtilsNOOP.h"

UnicodeTextUtilsNOOP::UnicodeTextUtilsNOOP() {
	UnicodeTextUtilsNOOP::instance = NULL;
}

AbstractUnicodeTextUtils* UnicodeTextUtilsNOOP::getInstance() {
	return instance;
}

EStatusCode UnicodeTextUtilsNOOP::getVisualString(
		const std::string& logicalString, /*OUT*/
		std::string& outVisualString, const std::string& charset) {
	outVisualString = logicalString;
	return eSuccess;
}

UnicodeTextUtilsNOOP::~UnicodeTextUtilsNOOP() {
	delete instance;
}

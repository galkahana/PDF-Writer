/*
 * UnicodeTextUtilsNOOP.cpp
 *
 *  Created on: Aug 21, 2016
 *      Author: amr
 */

#include "UnicodeTextUtilsNOOP.h"

UnicodeTextUtilsNOOP* UnicodeTextUtilsNOOP::instance;


AbstractUnicodeTextUtils* UnicodeTextUtilsNOOP::getInstance() {
	if( ! instance ) {
		instance = new UnicodeTextUtilsNOOP();
	}
	return instance;
}

EStatusCode UnicodeTextUtilsNOOP::getVisualString(
		const std::string& logicalString, /*OUT*/
		std::string& outVisualString, const std::string& charset) {
	outVisualString = logicalString;

	return eSuccess;
}

UnicodeTextUtilsNOOP::~UnicodeTextUtilsNOOP() {
	if( instance ) {
		delete instance;
		instance = NULL;
	}
}

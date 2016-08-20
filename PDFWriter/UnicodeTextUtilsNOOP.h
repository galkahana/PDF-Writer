/*
 * UnicodeTextUtilsNOOP.h
 *
 *  Created on: Aug 21, 2016
 *      Author: amr
 */

#ifndef PDFWRITER_UNICODETEXTUTILSNOOP_H_
#define PDFWRITER_UNICODETEXTUTILSNOOP_H_

#include "AbstractUnicodeTextUtils.h"

//This is a singleton class
class UnicodeTextUtilsNOOP: public AbstractUnicodeTextUtils {

	private:
	static UnicodeTextUtilsNOOP* instance;
	UnicodeTextUtilsNOOP();

	public:
	static AbstractUnicodeTextUtils* getInstance();
	virtual EStatusCode getVisualString(const std::string& logicalString, /*OUT*/ std::string& outVisualString, const std::string& charset="UTF-8");
	virtual ~UnicodeTextUtilsNOOP();

};

#endif /* PDFWRITER_UNICODETEXTUTILSNOOP_H_ */

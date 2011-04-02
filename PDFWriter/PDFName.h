#pragma once
#include "PDFObject.h"

#include <string>

using namespace std;

class PDFName : public PDFObject
{
public:
	// value must be the already interpreted name - no initial slash, and all special charachters (with # definition) interpreted
	PDFName(const string& inValue);
	virtual ~PDFName(void);

	const string& GetValue() const;
	operator string() const;

private:

	string mValue;
};

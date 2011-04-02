#pragma once

#include "PDFObject.h"

#include <string>

using namespace std;

class PDFLiteralString : public PDFObject
{
public:
	// Value is the interpreted string (no enclosing paranthesis and escaped sequances as the result values)
	PDFLiteralString(const string& inValue);
	virtual ~PDFLiteralString(void);

	const string& GetValue() const;
	operator string() const;

private:
	string mValue;

};

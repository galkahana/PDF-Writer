#pragma once

#include "PDFObject.h"

#include <string>

using namespace std;

class PDFHexString : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectHexString
	};

	// Value is the hex charactes string
	PDFHexString(const string& inValue);
	virtual ~PDFHexString(void);

	const string& GetValue() const;
	operator string() const;

private:
	string mValue;

};

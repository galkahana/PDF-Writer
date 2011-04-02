#pragma once

#include "PDFObject.h"

class PDFBoolean : public PDFObject
{
public:
	PDFBoolean(bool inValue);
	virtual ~PDFBoolean(void);

	bool GetValue() const;
	operator bool() const;


private:
	bool mValue;
};

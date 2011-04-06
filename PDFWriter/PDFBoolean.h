#pragma once

#include "PDFObject.h"

class PDFBoolean : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectBoolean
	};

	PDFBoolean(bool inValue);
	virtual ~PDFBoolean(void);

	bool GetValue() const;
	operator bool() const;


private:
	bool mValue;
};

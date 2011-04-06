#pragma once
#include "PDFObject.h"

#include <string>

using namespace std;

class PDFSymbol : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectSymbol
	};

	PDFSymbol(const string& inSymbol);
	virtual ~PDFSymbol(void);

	const string& GetValue() const;

private:

	string mValue;
};

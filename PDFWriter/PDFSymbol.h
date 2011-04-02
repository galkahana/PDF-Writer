#pragma once
#include "PDFObject.h"

#include <string>

using namespace std;

class PDFSymbol : public PDFObject
{
public:
	PDFSymbol(const string& inSymbol);
	virtual ~PDFSymbol(void);

	const string& GetValue() const;

private:

	string mValue;
};

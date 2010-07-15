#pragma once

#include <string>

using namespace std;

class PDFImageXObject;


class TIFFImageHandler
{
public:
	TIFFImageHandler(void);
	virtual ~TIFFImageHandler(void);

	PDFImageXObject* CreateImageXObjectFromTIFFFile(const wstring& inTIFFFilePath);
};

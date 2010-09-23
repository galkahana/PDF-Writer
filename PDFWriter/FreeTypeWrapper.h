#pragma once

#include "EStatusCode.h"

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

class FreeTypeWrapper
{
public:
	FreeTypeWrapper(void);
	~FreeTypeWrapper(void);

	FT_Face NewFace(const string& inFilePath,FT_Long inFontIndex = 0);
	FT_Face NewFace(const string& inFilePath,const string& inSecondaryFilePath,FT_Long inFontIndex = 0);
	FT_Face NewFace(const wstring& inFilePath,FT_Long inFontIndex = 0);
	FT_Face NewFace(const wstring& inFilePath,const wstring& inSecondaryFilePath,FT_Long inFontIndex = 0);
	FT_Error DoneFace(FT_Face ioFace);

	FT_Library operator->();
	operator FT_Library() const;

private:

	FT_Library mFreeType;

	FT_Stream CreateFTStreamForPath(const wstring& inFilePath);
	EStatusCode FillOpenFaceArgumentsForWideString(const wstring& inFilePath, FT_Open_Args& ioArgs);
	void CloseOpenFaceArgumentsStream(FT_Open_Args& ioArgs);

};

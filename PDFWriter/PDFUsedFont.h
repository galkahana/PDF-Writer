#pragma once

#include "FreeTypeFaceWrapper.h"
#include "ObjectsBasicTypes.h"
#include "EStatusCode.h"
#include <string>
#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

typedef list<unsigned short> UShortList;
typedef list<UShortList> UShortListList;
typedef list<wstring> WStringList;

class IWrittenFont;
class ObjectsContext;

class PDFUsedFont
{
public:
	PDFUsedFont(FT_Face inInputFace,const wstring& inAdditionalMetricsFontFilePath,ObjectsContext* inObjectsContext);
	virtual ~PDFUsedFont(void);

	bool IsValid();

	/*
		This function does the work of encoding a text string to a matching font instance name, and an encoded array
		of characters. the encoded array is a list of short values (double byte for CID, one byte for regular), and an extra
		boolean value indicates whether they are CID or regular.
	*/
	EStatusCode EncodeStringForShowing(const wstring& inText,
										ObjectIDType &outFontObjectToUse,
										UShortList& outCharactersToUse,
										bool& outTreatCharactersAsCID);

	// encode all strings. make sure that they will use the same font.
	EStatusCode EncodeStringsForShowing(const WStringList& inText,
										ObjectIDType &outFontObjectToUse,
										UShortListList& outCharactersToUse,
										bool& outTreatCharactersAsCID);

	EStatusCode WriteFontDefinition();

private:
	FreeTypeFaceWrapper mFaceWrapper;
	IWrittenFont* mWrittenFont;
	ObjectsContext* mObjectsContext;


};

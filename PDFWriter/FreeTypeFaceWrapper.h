#pragma once

#include "EFontStretch.h"
#include "EStatusCode.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <utility>
#include <list>
#include <string>

class IFreeTypeFaceExtender;
class IWrittenFont;
class ObjectsContext;

using namespace std;

typedef pair<bool,FT_Short> BoolAndFTShort;
typedef list<unsigned int> UIntList;
typedef list<UIntList> UIntListList;
typedef list<wstring> WStringList;

class FreeTypeFaceWrapper
{
public:
	// first overload - all but type ones
	FreeTypeFaceWrapper(FT_Face inFace);

	// second overload - type 1, to allow passing pfm file path. do not bother
	// if you don't have a PFM file. no i don't care about the godamn AFM file. just the PFM.
	// if you don't have a PFM i'll manage. again - i don't need the @#$@#$ AFM file. 
	// you see. i need to know if the font is serif, script 'n such. AFM - even if there
	// does not have that kind of info. so @#$@#$ off.
	// for any case, i'll check the file extension, and only do something about it if it has a pfm extension
	FreeTypeFaceWrapper(FT_Face inFace,const wstring& inPFMFilePath);
	~FreeTypeFaceWrapper(void);

	FT_Error DoneFace();

	FT_Face operator->();
	operator FT_Face();

	bool IsValid();

	EStatusCode GetGlyphsForUnicodeText(const wstring& inText,UIntList& outGlyphs);
	EStatusCode GetGlyphsForUnicodeText(const WStringList& inText,UIntListList& outGlyphs);

	double GetItalicAngle();
	BoolAndFTShort GetCapHeight();
	BoolAndFTShort GetxHeight();
	FT_UShort GetStemV();
	EFontStretch GetFontStretch();
	FT_UShort GetFontWeight();
	// these would be flags for the font as a whole. if subsetting, match to the character set
	unsigned int GetFontFlags();

	// Create the written font object, matching to write this font in the best way.
	IWrittenFont* CreateWrittenFontObject(ObjectsContext* inObjectsContext);


	// flags determining values
	bool IsFixedPitch();
	bool IsSerif();
	bool IsScript();
	bool IsItalic();
	bool IsForceBold();

	// will be used externally to determine if font is symbolic or not
	bool IsCharachterCodeAdobeStandard(FT_ULong inCharacterCode);

private:

	FT_Face mFace;
	IFreeTypeFaceExtender* mFormatParticularWrapper;
	bool mHaslowercase;

	wstring GetExtension(const wstring& inFilePath);
	void SetupFormatSpecificExtender(const wstring& inPFMFilePath);
	BoolAndFTShort CapHeightFromHHeight();
	BoolAndFTShort XHeightFromLowerXHeight();
	BoolAndFTShort GetYBearingForUnicodeChar(unsigned short unicodeCharCode);
	EFontStretch StretchFromName();
	FT_UShort WeightFromName();
	bool IsSymbolic();
	bool IsDefiningCharsNotInAdobeStandardLatin();
};

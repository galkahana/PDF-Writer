#include "PDFUsedFont.h"
#include "IWrittenFont.h"
#include "UnicodeEncoding.h"

PDFUsedFont::PDFUsedFont(FT_Face inInputFace,
						 const wstring& inFontFilePath,
						 const wstring& inAdditionalMetricsFontFilePath,
						 ObjectsContext* inObjectsContext):mFaceWrapper(inInputFace,inFontFilePath,inAdditionalMetricsFontFilePath)
{
	mObjectsContext = inObjectsContext;
	mWrittenFont = NULL;
}

PDFUsedFont::~PDFUsedFont(void)
{
	delete mWrittenFont;
}

bool PDFUsedFont::IsValid()
{
	return mFaceWrapper.IsValid();
}

EStatusCode PDFUsedFont::EncodeStringForShowing(const GlyphUnicodeMappingList& inText,
												ObjectIDType &outFontObjectToUse,
												UShortList& outCharactersToUse,
												bool& outTreatCharactersAsCID)
{
	if(!mWrittenFont)
		mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);

	mWrittenFont->AppendGlyphs(inText,outCharactersToUse,outTreatCharactersAsCID,outFontObjectToUse);

	return eSuccess;
}

EStatusCode PDFUsedFont::TranslateStringToGlyphs(const wstring& inText,GlyphUnicodeMappingList& outGlyphsUnicodeMapping)
{
	UIntList glyphs;
	UnicodeEncoding unicode;
	ULongVector unicodeCharacters;

	EStatusCode status = unicode.UTF16ToUnicode(inText,unicodeCharacters);
	if(status != eSuccess)
		return status;

	status = mFaceWrapper.GetGlyphsForUnicodeText(unicodeCharacters,glyphs);

	ULongVector::iterator itUnicode = unicodeCharacters.begin();
	UIntList::iterator itGlyphs = glyphs.begin();

	for(; itUnicode != unicodeCharacters.end(); ++itUnicode,++itGlyphs)
		outGlyphsUnicodeMapping.push_back(GlyphUnicodeMapping(*itGlyphs,*itUnicode));

	return status;
}

EStatusCode PDFUsedFont::EncodeStringsForShowing(const GlyphUnicodeMappingListList& inText,
												ObjectIDType &outFontObjectToUse,
												UShortListList& outCharactersToUse,
												bool& outTreatCharactersAsCID)
{
	if(!mWrittenFont)
		mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);

	mWrittenFont->AppendGlyphs(inText,outCharactersToUse,outTreatCharactersAsCID,outFontObjectToUse);

	return eSuccess;
}

EStatusCode PDFUsedFont::WriteFontDefinition()
{
	return mWrittenFont->WriteFontDefinition(mFaceWrapper);
}
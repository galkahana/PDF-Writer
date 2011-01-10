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

EStatusCode PDFUsedFont::EncodeStringForShowing(const wstring& inText,
												ObjectIDType &outFontObjectToUse,
												UShortList& outCharactersToUse,
												bool& outTreatCharactersAsCID)
{
	// K. let's assume that arrive here after validating that all is well. m'k? getting tired of making
	// all these theoretical validations that all is well. if you really must do this - add check that written font is non null after Creation.
	UIntList glyphs;
	UnicodeEncoding unicode;
	ULongVector unicodeCharacters;

	EStatusCode status = unicode.UTF16ToUnicode(inText,unicodeCharacters);
	if(status != eSuccess)
		return status;

	status = mFaceWrapper.GetGlyphsForUnicodeText(unicodeCharacters,glyphs);

	if(!mWrittenFont)
		mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);

	mWrittenFont->AppendGlyphs(glyphs,unicodeCharacters,outCharactersToUse,outTreatCharactersAsCID,outFontObjectToUse);

	return status;
}

EStatusCode PDFUsedFont::EncodeStringsForShowing(const WStringList& inText,
									ObjectIDType &outFontObjectToUse,
									UShortListList& outCharactersToUse,
									bool& outTreatCharactersAsCID)
{
	UIntListList glyphs;

	UnicodeEncoding unicode;
	ULongVector unicodeCharacters;
	ULongVectorList unicodeCharactersList;

	EStatusCode status = eSuccess;
	WStringList::const_iterator it = inText.begin();

	for(; it != inText.end() && eSuccess == status;++it)
	{
		status = unicode.UTF16ToUnicode(*it,unicodeCharacters);
		unicodeCharactersList.push_back(unicodeCharacters);
	}
	if(status != eSuccess)
		return status;


	status = mFaceWrapper.GetGlyphsForUnicodeText(unicodeCharactersList,glyphs);

	if(!mWrittenFont)
		mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);

	mWrittenFont->AppendGlyphs(glyphs,unicodeCharactersList,outCharactersToUse,outTreatCharactersAsCID,outFontObjectToUse);

	return status;
}

EStatusCode PDFUsedFont::WriteFontDefinition()
{
	return mWrittenFont->WriteFontDefinition(mFaceWrapper);
}
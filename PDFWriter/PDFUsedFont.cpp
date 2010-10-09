#include "PDFUsedFont.h"
#include "IWrittenFont.h"

PDFUsedFont::PDFUsedFont(FT_Face inInputFace,ObjectsContext* inObjectsContext):mFaceWrapper(inInputFace)
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
	EStatusCode status = mFaceWrapper.GetGlyphsForUnicodeText(inText,glyphs);

	if(!mWrittenFont)
		mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);

	mWrittenFont->AppendGlyphs(glyphs,inText,outCharactersToUse,outTreatCharactersAsCID,outFontObjectToUse);

	return status;
}

EStatusCode PDFUsedFont::WriteFontDefinition()
{
	return mWrittenFont->WriteFontDefinition(mFaceWrapper);
}
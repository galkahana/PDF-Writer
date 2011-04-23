/*
   Source File : PDFUsedFont.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "PDFUsedFont.h"
#include "IWrittenFont.h"
#include "UnicodeEncoding.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFIndirectObjectReference.h"

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

EStatusCode PDFUsedFont::WriteState(ObjectsContext* inStateWriter,ObjectIDType inObjectID)
{
	inStateWriter->StartNewIndirectObject(inObjectID);
	DictionaryContext* pdfUsedFontObject = inStateWriter->StartDictionary();

	pdfUsedFontObject->WriteKey("Type");
	pdfUsedFontObject->WriteNameValue("PDFUsedFont");

	ObjectIDType writtenFontObject;

	if(mWrittenFont)
	{
		writtenFontObject = inStateWriter->GetInDirectObjectsRegistry().AllocateNewObjectID();

		pdfUsedFontObject->WriteKey("mWrittenFont");
		pdfUsedFontObject->WriteObjectReferenceValue(writtenFontObject);
	}

	inStateWriter->EndDictionary(pdfUsedFontObject);
	inStateWriter->EndIndirectObject();

	if(mWrittenFont)
		mWrittenFont->WriteState(inStateWriter,writtenFontObject);

	return eSuccess;
}

EStatusCode PDFUsedFont::ReadState(PDFParser* inStateReader,ObjectIDType inObjectID)
{
	PDFObjectCastPtr<PDFDictionary> pdfUsedFontState(inStateReader->ParseNewObject(inObjectID));

	PDFObjectCastPtr<PDFIndirectObjectReference> writtenFontReference(pdfUsedFontState->QueryDirectObject("mWrittenFont"));

	if(!writtenFontReference)
		return eSuccess;

	if(mWrittenFont)
		delete mWrittenFont;

	mWrittenFont = mFaceWrapper.CreateWrittenFontObject(mObjectsContext);
	if(!mWrittenFont)
		return eFailure;

	return mWrittenFont->ReadState(inStateReader,writtenFontReference->mObjectID);
}

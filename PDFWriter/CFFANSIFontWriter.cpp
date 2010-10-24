#include "CFFANSIFontWriter.h"
#include "ANSIFontWriter.h"
#include "DictionaryContext.h"
#include "IByteWriterWithPosition.h"
#include "PrimitiveObjectsWriter.h"
#include "PDFStream.h"
#include "SafeBufferMacrosDefs.h"
#include "ObjectsContext.h"
#include "FreeTypeFaceWrapper.h"

#include <ft2build.h>
#include FT_FREETYPE_H

CFFANSIFontWriter::CFFANSIFontWriter(void)
{
}

CFFANSIFontWriter::~CFFANSIFontWriter(void)
{
}

EStatusCode CFFANSIFontWriter::WriteFont(	FreeTypeFaceWrapper& inFontInfo,
											WrittenFontRepresentation* inFontOccurrence,
											ObjectsContext* inObjectsContext)
{
	ANSIFontWriter fontWriter;

	return fontWriter.WriteFont(inFontInfo,inFontOccurrence,inObjectsContext,this);


	// TODO : need to write the CFF font program
}

static const string scType1 = "Type1";

void CFFANSIFontWriter::WriteSubTypeValue(DictionaryContext* inDictionary)
{
	inDictionary->WriteNameValue(scType1);
}

IFontDescriptorHelper* CFFANSIFontWriter::GetCharsetWriter()
{
	return this;
}


static const string scCharSet = "CharSet";
static const Byte scLeftParanthesis[] = {'('};
static const Byte scRightParanthesis[] = {')'};

void CFFANSIFontWriter::WriteCharSet(	DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext,
										FreeTypeFaceWrapper* inFontInfo,
										const UIntAndGlyphEncodingInfoVector& inEncodedGlyphs)
{
	// ChartSet
	inDescriptorContext->WriteKey(scCharSet);

	// charset is a rather long string. i'd rather write it down as it goes, than format a string
	// and submit it.
	IByteWriterWithPosition* directStream = inObjectsContext->StartFreeContext();
	PrimitiveObjectsWriter primitiveWriter(directStream);

	directStream->Write(scLeftParanthesis,1);

	UIntAndGlyphEncodingInfoVector::const_iterator it = inEncodedGlyphs.begin()+1; // skip 0 character
	char buffer[100];
	
	for(; it != inEncodedGlyphs.end(); ++it)
	{
		FT_Get_Glyph_Name(*inFontInfo,it->first,buffer,100);
		primitiveWriter.WriteName(buffer,eTokenSepratorNone);
	}
	directStream->Write(scRightParanthesis,1);
	inObjectsContext->EndFreeContext();
	inObjectsContext->EndLine();
}

static const string scFontFile3 = "FontFile3";
void CFFANSIFontWriter::WriteFontFileReference(	
										DictionaryContext* inDescriptorContext,
										ObjectsContext* inObjectsContext)
{
	// FontFile3
	inDescriptorContext->WriteNameValue(scFontFile3);
	mEmbeddedFontFileObjectID = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID();
	inDescriptorContext->WriteObjectReferenceValue(mEmbeddedFontFileObjectID);
}

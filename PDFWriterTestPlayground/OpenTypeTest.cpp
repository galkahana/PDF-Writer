/*
   Source File : OpenTypeTest.cpp


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
#include "OpenTypeTest.h"
#include "OpenTypeFileInput.h"
#include "InputFile.h"
#include "TestsRunner.h"

#include "BoxingBase.h"
#include "OutputFile.h"
#include "CharStringType2Tracer.h"
#include "IByteWriterWithPosition.h"

#include <iostream>


OpenTypeTest::OpenTypeTest(void)
{
}

OpenTypeTest::~OpenTypeTest(void)
{
}


#define FONT_OBJECT_START 1276 + 84
#define FONT_OBJECT_STREAM_LENGTH 1021

#include "OutputFlateDecodeStream.h"
#include "OutputStreamTraits.h"

EStatusCode OpenTypeTest::ExtractFontSegment()
{
	InputFile sourcePdfFile;
	OutputFlateDecodeStream decoderStream;

	EStatusCode status = sourcePdfFile.OpenFile(L"c:\\pdflibtests\\SimpleTextUsageType1.PDF");
	do
	{
		if(status != eSuccess)
			break;

		OutputFile outputFile;

		status = outputFile.OpenFile(L"c:\\pdflibtests\\SimpleType1Segment.cff");
		decoderStream.Assign(outputFile.GetOutputStream());

		OutputStreamTraits traits(&decoderStream);

		sourcePdfFile.GetInputStream()->SetPosition(FONT_OBJECT_START);

		status = traits.CopyToOutputStream(sourcePdfFile.GetInputStream(),FONT_OBJECT_STREAM_LENGTH);
		decoderStream.Assign(NULL);
		if(status != eSuccess)
			break;
	}
	while(false);

	return status;
}

#include "StringTraits.h"

EStatusCode OpenTypeTest::DisplayFontSegmentInformation()
{
	InputFile inputFile;

	EStatusCode status = inputFile.OpenFile(L"c:\\pdflibtests\\SimpleType1Segment.cff");
		
	do
	{
		if(status != eSuccess)
			break;

		CFFFileInput cffFileInput;

		status = cffFileInput.ReadCFFFile(inputFile.GetInputStream(),0);
		if(status != eSuccess)
			break;


		// dumping font dictionary information
		wcout<<"Dumping font information for "<<StringTraits(cffFileInput.mName.front()).WidenString().c_str()<<"\n";
		wcout<<"Top Dict:\n";
		DumpDictionary(cffFileInput.mTopDictIndex[0].mTopDict);

		wcout<<"\nPrivate Dict:\n";
		DumpDictionary(cffFileInput.mPrivateDicts[0].mPrivateDict);

		wcout<<"\nSaving Glyph programs for a,b,c,d and .notdef now...\n";
		// a,b,c,d and .notdef
		for(unsigned short i=0; i < 5 && eSuccess == status; ++i)
			status = SaveCharstringCode(0,i,&cffFileInput);
		wcout<<"Done\n";
	}while(false);

	return status;
}

void OpenTypeTest::DumpDictionary(const UShortToDictOperandListMap& inDict)
{
	UShortToDictOperandListMap::const_iterator it = inDict.begin();

	for(; it != inDict.end(); ++it)
	{
		wcout<<FormatDictionaryKey(it->first).c_str()<<L" = ";
		if(it->second.size() == 1)
		{
			wcout<<(it->second.front().IsInteger ? 
					Long(it->second.front().IntegerValue).ToWString() : 
					Double(it->second.front().RealValue).ToWString())
				 <<L"\n";
		}
		else
		{
			wcout<<L"[ ";
			DictOperandList::const_iterator itOperands = it->second.begin();
			for(; itOperands != it->second.end(); ++itOperands)
			{
				wcout<<(itOperands->IsInteger ? 
					Long(itOperands->IntegerValue).ToWString() : 
					Double(itOperands->RealValue).ToWString())<<L" ";
			}
			wcout<<L"]\n";
		}
	}
}

static const wchar_t* scLowBytesOperands[22] =
{
	L"version",
	L"Notice",
	L"FullName",
	L"FamilyName",
	L"Weight",	
	L"FontBBox",
	L"BlueValues",
	L"OtherBlues",
	L"FamilyBlues",
	L"FamilyOtherBlues",
	L"StdHW",
	L"StdVW",
	L"Escape",
	L"UniqueID",
	L"XUID",
	L"charset",
	L"Encoding",
	L"CharStrings",
	L"Private",
	L"Subrs",
	L"defaultWidthX",
	L"nominalWidthX"
};

static const wchar_t* scHighBytesOperands[23] =
{
	L"Copyright",
	L"isFixedPitch",
	L"ItalicAngle",
	L"UnderlinePosition",
	L"UnderlineThickness",
	L"PaintType",
	L"CharstringType",
	L"FontMatrix",
	L"StrokeWidth",
	L"BlueScale",
	L"BlueShift",
	L"BlueFuzz",
	L"StemSnapH",
	L"StemSnapV",
	L"ForceBold",
	L"ForceBoldThreshold",
	L"lenIV",
	L"LanguageGroup",
	L"ExpansionFactor",
	L"initialRandomSeed",
	L"SyntheticBase",
	L"PostScript",
	L"BaseFontName"
};

#include "SafeBufferMacrosDefs.h"

wstring OpenTypeTest::FormatDictionaryKey(unsigned short inDictionaryKey)
{
	Byte highByte = (Byte)((inDictionaryKey >> 8) & 0xff);
	Byte lowByte = (Byte)(inDictionaryKey & 0xff);
	wstring result;

	if(0 == highByte)
	{
		if(lowByte < 22)
			result = scLowBytesOperands[lowByte];
		else
			result = Int(inDictionaryKey).ToWString();
	}
	else if(12 == highByte)
	{
		if(lowByte < 23)
			result = scHighBytesOperands[lowByte];
		else
		{
			wchar_t buffer[10];
			result = SAFE_SWPRINTF_1(buffer,10,L"12 %d",lowByte);
		}
	}
	else
	{
		result = L"Error";
	}
	return result;
}


EStatusCode OpenTypeTest::SaveCharstringCode(unsigned short inFontIndex,unsigned short inGlyphIndex,CFFFileInput* inCFFFileInput)
{
	OutputFile glyphFile;

	EStatusCode status = glyphFile.OpenFile(wstring(L"C:\\PDFLibTests\\glyphCFF")  + Long(inFontIndex).ToWString() + L"_" + StringTraits(inCFFFileInput->GetGlyphName(0,inGlyphIndex)).WidenString() + L".txt");

	do
	{
		if(status != eSuccess)
			break;
		
		CharStringType2Tracer tracer;

		status = tracer.TraceGlyphProgram(inFontIndex,inGlyphIndex,inCFFFileInput,glyphFile.GetOutputStream());

	}while(false);

	glyphFile.CloseFile();

	return status;
}

EStatusCode OpenTypeTest::Run()
{
	//return ExtractFontSegment();
	//return DisplayFontSegmentInformation();
	return TestFont();
}

EStatusCode OpenTypeTest::TestFont()
{
	EStatusCode status;
	InputFile otfFile;

	do
	{
		status = otfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\BrushScriptStd.otf");

		if(status != eSuccess)
		{
			wcout<<"cannot read bursh script font file\n";
			break;
		}


		OpenTypeFileInput openTypeReader;

		status = openTypeReader.ReadOpenTypeFile(otfFile.GetInputStream());
		if(status != eSuccess)
		{
			wcout<<"could not read open type file\n";
			break;
		}

		/*
		unsigned short glyphsCount = openTypeReader.mCFF.GetCharStringsCount(0);
		for(unsigned short i=0; i < glyphsCount && eSuccess == status; ++i)
		{
			CharString2Dependencies dependencies;
			status = openTypeReader.mCFF.CalculateDependenciesForCharIndex(0,i,dependencies);
			if(status != eSuccess)
			{
				wcout<<"count not calculate dependencies for glyph index "<<i<<"\n";
			}
			else
			{
				if(dependencies.mLocalSubrs.size() != 0 || dependencies.mGlobalSubrs.size() != 0)
				{
					wcout<<"Glyph index = "<<i<<", Named "<<openTypeReader.mCFF.GetGlyphName(0,i).c_str()<<" has subroutine dependencies:\n";
					UShortSet::iterator it;
					for(it = dependencies.mGlobalSubrs.begin(); it != dependencies.mGlobalSubrs.end(); ++it)
						wcout<<"Global Subr, Index = "<<*it<<"\n";
					for(it = dependencies.mLocalSubrs.begin(); it != dependencies.mLocalSubrs.end(); ++it)
						wcout<<"Local Subr, Index = "<<*it<<"\n";
				}
				else
				{
					wcout<<"No subrs dependencies for Glyph index = "<<i<<", Named "<<openTypeReader.mCFF.GetGlyphName(0,i).c_str()<<"\n";
				}
			}
		}*/

		// show just abcd and notdef

		status = SaveCharstringCode(0,0,&openTypeReader.mCFF);
		for(unsigned short i=66; i < 70 && eSuccess == status; ++i)
			status = SaveCharstringCode(0,i,&openTypeReader.mCFF);
	}while(false);
	return status;
}

ADD_CETEGORIZED_TEST(OpenTypeTest,"OpenType")
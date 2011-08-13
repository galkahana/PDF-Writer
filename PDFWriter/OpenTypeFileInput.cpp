/*
   Source File : OpenTypeFileInput.cpp


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
#include "OpenTypeFileInput.h"
#include "Trace.h"
#include "InputFile.h"

using namespace PDFHummus;

OpenTypeFileInput::OpenTypeFileInput(void)
{
	mHMtx = NULL;
	mName.mNameEntries = NULL;
	mLoca = NULL;
	mGlyf = NULL;
}

OpenTypeFileInput::~OpenTypeFileInput(void)
{
	FreeTables();
}

void OpenTypeFileInput::FreeTables()
{
	delete[] mHMtx;
	mHMtx = NULL;
	if(mName.mNameEntries)
	{
		for(unsigned short i =0; i < mName.mNameEntriesCount; ++i)
			delete[] mName.mNameEntries[i].String;
	}
	delete[] mName.mNameEntries;
	mName.mNameEntries = NULL;
	delete[] mLoca;
	mLoca = NULL;
	delete[] mGlyf;
	mGlyf = NULL;

	UShortToGlyphEntryMap::iterator it = mActualGlyphs.begin();
	for(; it != mActualGlyphs.end(); ++it)
		delete it->second;
	mActualGlyphs.clear();
}

EStatusCode OpenTypeFileInput::ReadOpenTypeFile(const string& inFontFilePath)
{
	InputFile fontFile;

	EStatusCode status = fontFile.OpenFile(inFontFilePath);
	if(status != PDFHummus::eSuccess)
	{
		TRACE_LOG1("OpenTypeFileInput::ReadOpenTypeFile, cannot open true type font file at %s",inFontFilePath.c_str());
		return status;
	}

	status = ReadOpenTypeFile(fontFile.GetInputStream());
	fontFile.CloseFile();
	return status;
}

EStatusCode OpenTypeFileInput::ReadOpenTypeFile(IByteReaderWithPosition* inTrueTypeFile)
{
	EStatusCode status;

	do
	{
		FreeTables();

		mPrimitivesReader.SetOpenTypeStream(inTrueTypeFile);

		status = ReadOpenTypeHeader();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read true type header");
			break;
		}
		
		status = ReadHead();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read head table");
			break;
		}
		
		status = ReadMaxP();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read maxp table");
			break;
		}

		status = ReadHHea();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read hhea table");
			break;
		}

		status = ReadHMtx();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read hmtx table");
			break;
		}

		status = ReadOS2();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read os2 table");
			break;
		}

		status = ReadName();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read name table");
			break;
		}

		if(EOpenTypeTrueType == mFontType)
		{
			// true type specifics
			status = ReadLoca();
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read loca table");
				break;
			}

			status = ReadGlyfForDependencies();
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read glyf table");
				break;
			}
			mCVTExists = mTables.find(GetTag("cvt ")) != mTables.end();
			mFPGMExists = mTables.find(GetTag("fpgm")) != mTables.end();
			mPREPExists = mTables.find(GetTag("prep")) != mTables.end();

			// zero cff items
			mCFF.Reset();
		}
		else
		{
			// CFF specifics
			status = ReadCFF();
			if(status != PDFHummus::eSuccess)
			{
				TRACE_LOG("OpenTypeFileInput::ReadOpenTypeFile, failed to read CFF table");
			}

			// zero true type items
			mCVTExists = false;
			mFPGMExists = false;
			mPREPExists = false;
			mGlyf = NULL;
			mLoca = NULL;
		}
	}while(false);

	return status;
}

EStatusCode OpenTypeFileInput::ReadOpenTypeHeader()
{
	EStatusCode status;
	TableEntry tableEntry;
	unsigned long tableTag;

	do
	{
		status = ReadOpenTypeSFNT();
		if(status != PDFHummus::eSuccess)
		{
			TRACE_LOG("OpenTypeFileInput::ReaderTrueTypeHeader, SFNT header not open type");
			break;
		}

		mPrimitivesReader.ReadUSHORT(mTablesCount);
		// skip the next 6. i don't give a rats...
		mPrimitivesReader.Skip(6);

		for(unsigned short i = 0; i < mTablesCount; ++i)
		{
			mPrimitivesReader.ReadULONG(tableTag);
			mPrimitivesReader.ReadULONG(tableEntry.CheckSum);
			mPrimitivesReader.ReadULONG(tableEntry.Offset);
			mPrimitivesReader.ReadULONG(tableEntry.Length);
			mTables.insert(ULongToTableEntryMap::value_type(tableTag,tableEntry));
		}
		status = mPrimitivesReader.GetInternalState();
	}
	while(false);

	return status;
}

EStatusCode OpenTypeFileInput::ReadOpenTypeSFNT()
{
	unsigned long sfntVersion;

	mPrimitivesReader.ReadULONG(sfntVersion);

	if((0x10000 == sfntVersion) || (0x74727565 /* true */ == sfntVersion))
	{
		mFontType = EOpenTypeTrueType;
		return PDFHummus::eSuccess;
	}
	else if(0x4F54544F /* OTTO */ == sfntVersion)
	{
		mFontType = EOpenTypeCFF;
		return PDFHummus::eSuccess;
	}
	else
		return PDFHummus::eFailure;
}

unsigned long OpenTypeFileInput::GetTag(const char* inTagName)
{
	Byte buffer[4];
	unsigned short i=0;

	for(; i<strlen(inTagName);++i)
		buffer[i] = (Byte)inTagName[i];
	for(;i<4;++i)
		buffer[i] = 0x20;

	return	((unsigned long)buffer[0]<<24) + ((unsigned long)buffer[1]<<16) + 
			((unsigned long)buffer[2]<<8) + buffer[3];
}

EStatusCode OpenTypeFileInput::ReadHead()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("head"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadHead, could not find head table");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);
	mPrimitivesReader.ReadFixed(mHead.TableVersionNumber);
	mPrimitivesReader.ReadFixed(mHead.FontRevision);
	mPrimitivesReader.ReadULONG(mHead.CheckSumAdjustment);
	mPrimitivesReader.ReadULONG(mHead.MagicNumber);
	mPrimitivesReader.ReadUSHORT(mHead.Flags);
	mPrimitivesReader.ReadUSHORT(mHead.UnitsPerEm);
	mPrimitivesReader.ReadLongDateTime(mHead.Created);
	mPrimitivesReader.ReadLongDateTime(mHead.Modified);
	mPrimitivesReader.ReadSHORT(mHead.XMin);
	mPrimitivesReader.ReadSHORT(mHead.YMin);
	mPrimitivesReader.ReadSHORT(mHead.XMax);
	mPrimitivesReader.ReadSHORT(mHead.YMax);
	mPrimitivesReader.ReadUSHORT(mHead.MacStyle);
	mPrimitivesReader.ReadUSHORT(mHead.LowerRectPPEM);
	mPrimitivesReader.ReadSHORT(mHead.FontDirectionHint);
	mPrimitivesReader.ReadSHORT(mHead.IndexToLocFormat);
	mPrimitivesReader.ReadSHORT(mHead.GlyphDataFormat);

	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadMaxP()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("maxp"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadMaxP, could not find maxp table");
		return PDFHummus::eFailure;
	}
	mPrimitivesReader.SetOffset(it->second.Offset);

	memset(&mMaxp,0,sizeof(MaxpTable)); // set all with 0's in case the table's too short, so we'll have nice lookin values

	mPrimitivesReader.ReadFixed(mMaxp.TableVersionNumber);
	mPrimitivesReader.ReadUSHORT(mMaxp.NumGlyphs);

	if(1.0 == mMaxp.TableVersionNumber)
	{
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxPoints);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxCountours);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxCompositePoints);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxCompositeContours);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxZones);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxTwilightPoints);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxStorage);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxFunctionDefs);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxInstructionDefs);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxStackElements);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxSizeOfInstructions);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxComponentElements);
		mPrimitivesReader.ReadUSHORT(mMaxp.MaxCompontentDepth);
	}
	return mPrimitivesReader.GetInternalState();	

}

EStatusCode OpenTypeFileInput::ReadHHea()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("hhea"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadHHea, could not find hhea table");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);

	mPrimitivesReader.ReadFixed(mHHea.TableVersionNumber);
	mPrimitivesReader.ReadSHORT(mHHea.Ascender);
	mPrimitivesReader.ReadSHORT(mHHea.Descender);
	mPrimitivesReader.ReadSHORT(mHHea.LineGap);
	mPrimitivesReader.ReadUSHORT(mHHea.AdvanceWidthMax);
	mPrimitivesReader.ReadSHORT(mHHea.MinLeftSideBearing);
	mPrimitivesReader.ReadSHORT(mHHea.MinRightSideBearing);
	mPrimitivesReader.ReadSHORT(mHHea.XMaxExtent);
	mPrimitivesReader.ReadSHORT(mHHea.CaretSlopeRise);
	mPrimitivesReader.ReadSHORT(mHHea.CaretSlopeRun);
	mPrimitivesReader.ReadSHORT(mHHea.CaretOffset);
	mPrimitivesReader.Skip(8);
	mPrimitivesReader.ReadSHORT(mHHea.MetricDataFormat);
	mPrimitivesReader.ReadUSHORT(mHHea.NumberOfHMetrics);

	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadHMtx()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("hmtx"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadHMtx, could not find hmtx table");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);

	mHMtx = new HMtxTableEntry[mMaxp.NumGlyphs];

	unsigned int i=0;

	for(; i < mHHea.NumberOfHMetrics;++i)
	{
		mPrimitivesReader.ReadUSHORT(mHMtx[i].AdvanceWidth);
		mPrimitivesReader.ReadSHORT(mHMtx[i].LeftSideBearing);
	}

	for(; i < mMaxp.NumGlyphs; ++i)
	{
		mHMtx[i].AdvanceWidth = mHMtx[mHHea.NumberOfHMetrics-1].AdvanceWidth;
		mPrimitivesReader.ReadSHORT(mHMtx[i].LeftSideBearing);
	}

	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadOS2()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("OS/2"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadOS2, could not find os2 table");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);

	memset(&mOS2,0,sizeof(OS2Table));

	mPrimitivesReader.ReadUSHORT(mOS2.Version);
	mPrimitivesReader.ReadSHORT(mOS2.AvgCharWidth);
	mPrimitivesReader.ReadUSHORT(mOS2.WeightClass);
	mPrimitivesReader.ReadUSHORT(mOS2.WidthClass);
	mPrimitivesReader.ReadUSHORT(mOS2.fsType);

	mPrimitivesReader.ReadSHORT(mOS2.SubscriptXSize);
	mPrimitivesReader.ReadSHORT(mOS2.SubscriptYSize);
	mPrimitivesReader.ReadSHORT(mOS2.SubscriptXOffset);
	mPrimitivesReader.ReadSHORT(mOS2.SubscriptYOffset);
	mPrimitivesReader.ReadSHORT(mOS2.SuperscriptXSize);
	mPrimitivesReader.ReadSHORT(mOS2.SuperscriptYSize);
	mPrimitivesReader.ReadSHORT(mOS2.SuperscriptXOffset);
	mPrimitivesReader.ReadSHORT(mOS2.SuperscriptYOffset);
	mPrimitivesReader.ReadSHORT(mOS2.StrikeoutSize);
	mPrimitivesReader.ReadSHORT(mOS2.StrikeoutPosition);
	mPrimitivesReader.ReadSHORT(mOS2.FamilyClass);
	for(int i=0; i <10; ++i)
		mPrimitivesReader.ReadBYTE(mOS2.Panose[i]);
	mPrimitivesReader.ReadULONG(mOS2.UnicodeRange1);
	mPrimitivesReader.ReadULONG(mOS2.UnicodeRange2);
	mPrimitivesReader.ReadULONG(mOS2.UnicodeRange3);
	mPrimitivesReader.ReadULONG(mOS2.UnicodeRange4);
	for(int i=0; i <4; ++i)
		mPrimitivesReader.ReadCHAR(mOS2.AchVendID[i]);
	mPrimitivesReader.ReadUSHORT(mOS2.FSSelection);
	mPrimitivesReader.ReadUSHORT(mOS2.FirstCharIndex);
	mPrimitivesReader.ReadUSHORT(mOS2.LastCharIndex);
	mPrimitivesReader.ReadSHORT(mOS2.TypoAscender);
	mPrimitivesReader.ReadSHORT(mOS2.TypoDescender);
	mPrimitivesReader.ReadSHORT(mOS2.TypoLineGap);
	mPrimitivesReader.ReadUSHORT(mOS2.WinAscent);
	mPrimitivesReader.ReadUSHORT(mOS2.WinDescent);

	// version 1 OS/2 table may end here [see that there's enough to continue]
	if(it->second.Length >= (mPrimitivesReader.GetCurrentPosition() - it->second.Offset) + 18)
	{
		mPrimitivesReader.ReadULONG(mOS2.CodePageRange1);
		mPrimitivesReader.ReadULONG(mOS2.CodePageRange2);
		mPrimitivesReader.ReadSHORT(mOS2.XHeight);
		mPrimitivesReader.ReadSHORT(mOS2.CapHeight);
		mPrimitivesReader.ReadUSHORT(mOS2.DefaultChar);
		mPrimitivesReader.ReadUSHORT(mOS2.BreakChar);
		mPrimitivesReader.ReadUSHORT(mOS2.MaxContext);
	}
	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadName()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("name"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadName, could not find name table");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);	
	mPrimitivesReader.Skip(2);
	mPrimitivesReader.ReadUSHORT(mName.mNameEntriesCount);
	mName.mNameEntries = new NameTableEntry[mName.mNameEntriesCount];
	
	unsigned short stringOffset;
	
	mPrimitivesReader.ReadUSHORT(stringOffset);

	for(unsigned short i=0;i<mName.mNameEntriesCount;++i)
	{
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].PlatformID);
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].EncodingID);
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].LanguageID);
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].NameID);
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].Length);
		mPrimitivesReader.ReadUSHORT(mName.mNameEntries[i].Offset);
	}

	for(unsigned short i=0;i<mName.mNameEntriesCount;++i)
	{
		mName.mNameEntries[i].String = new char[mName.mNameEntries[i].Length];
		mPrimitivesReader.SetOffset(it->second.Offset + stringOffset + mName.mNameEntries[i].Offset);
		mPrimitivesReader.Read((Byte*)(mName.mNameEntries[i].String),mName.mNameEntries[i].Length);
	}

	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadLoca()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("loca"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadLoca, could not find loca table");
		return PDFHummus::eFailure;
	}
	mPrimitivesReader.SetOffset(it->second.Offset);	

	mLoca = new unsigned long[mMaxp.NumGlyphs+1];

	if(0 == mHead.IndexToLocFormat)
	{
		unsigned short buffer;
		for(unsigned short i=0; i < mMaxp.NumGlyphs+1; ++i)
		{
			mPrimitivesReader.ReadUSHORT(buffer);
			mLoca[i] = buffer << 1;
		}
	}
	else
	{
		for(unsigned short i=0; i < mMaxp.NumGlyphs+1; ++i)
			mPrimitivesReader.ReadULONG(mLoca[i]);
	}
	return mPrimitivesReader.GetInternalState();	
}

EStatusCode OpenTypeFileInput::ReadGlyfForDependencies()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("glyf"));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadGlyfForDependencies, could not find glyf table");
		return PDFHummus::eFailure;
	}

	// it->second.Offset, is the offset to the beginning of the table
	mGlyf = new GlyphEntry*[mMaxp.NumGlyphs];

	for(unsigned short i=0; i < mMaxp.NumGlyphs; ++i)
	{
		if(mLoca[i+1] == mLoca[i])
		{
			mGlyf[i] = NULL;
		}
		else
		{
			mGlyf[i] = new GlyphEntry;

			mPrimitivesReader.SetOffset(it->second.Offset + mLoca[i]);
			mPrimitivesReader.ReadSHORT(mGlyf[i]->NumberOfContours);
			mPrimitivesReader.ReadSHORT(mGlyf[i]->XMin);
			mPrimitivesReader.ReadSHORT(mGlyf[i]->YMin);
			mPrimitivesReader.ReadSHORT(mGlyf[i]->XMax);
			mPrimitivesReader.ReadSHORT(mGlyf[i]->YMax);

			// Now look for dependencies
			if(mGlyf[i]->NumberOfContours < 0)
			{
				bool hasMoreComponents;
				unsigned short flags;
				unsigned short glyphIndex;

				do
				{
					mPrimitivesReader.ReadUSHORT(flags);
					mPrimitivesReader.ReadUSHORT(glyphIndex);
					mGlyf[i]->mComponentGlyphs.push_back(glyphIndex);
					if((flags & 1) != 0) // 
						mPrimitivesReader.Skip(4); // skip 2 shorts, ARG_1_AND_2_ARE_WORDS
					else
						mPrimitivesReader.Skip(2); // skip 1 short, nah - they are bytes

					if((flags & 8) != 0)
						mPrimitivesReader.Skip(2); // WE_HAVE_SCALE
					else if ((flags & 64) != 0)
						mPrimitivesReader.Skip(4); // WE_HAVE_AN_X_AND_Y_SCALE
					else if ((flags & 128) != 0)
						mPrimitivesReader.Skip(8); // WE_HAVE_A_TWO_BY_TWO

					hasMoreComponents = ((flags & 32) != 0);
				}while(hasMoreComponents);

			}

			mActualGlyphs.insert(UShortToGlyphEntryMap::value_type(i,mGlyf[i]));
		}
	}	


	return mPrimitivesReader.GetInternalState();	
}

unsigned short OpenTypeFileInput::GetGlyphsCount()
{
	return mMaxp.NumGlyphs;
}

TableEntry* OpenTypeFileInput::GetTableEntry(const char* inTagName)
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag(inTagName));
	
	if(it == mTables.end())	
		return NULL;
	else
		return &(it->second);
}

EStatusCode OpenTypeFileInput::ReadCFF()
{
	ULongToTableEntryMap::iterator it = mTables.find(GetTag("CFF "));
	if(it == mTables.end())
	{
		TRACE_LOG("OpenTypeFileInput::ReadCFF, could not find cff table entry");
		return PDFHummus::eFailure;
	}

	mPrimitivesReader.SetOffset(it->second.Offset);

	return mCFF.ReadCFFFile(mPrimitivesReader.GetReadStream());
}

EOpenTypeInputType OpenTypeFileInput::GetOpenTypeFontType()
{
	return mFontType;
}
#pragma once

#include "EStatusCode.h"
#include "OpenTypePrimitiveReader.h"
#include "IByteReaderWithPosition.h"
#include "CFFFileInput.h"

#include <string>
#include <map>
#include <list>

using namespace std;

struct TableEntry
{
	unsigned long CheckSum;
	unsigned long Offset;
	unsigned long Length;
};

typedef map<unsigned long,TableEntry> ULongToTableEntryMap;


struct HeadTable
{
	double TableVersionNumber;
	double FontRevision;
	unsigned long CheckSumAdjustment;
	unsigned long MagicNumber;
	unsigned short Flags;
	unsigned short UnitsPerEm;
	long long Created;
	long long Modified;
	short XMin;
	short YMin;
	short XMax;
	short YMax;
	unsigned short MacStyle;
	unsigned short LowerRectPPEM;
	short FontDirectionHint;
	short IndexToLocFormat;
	short GlyphDataFormat;
};

struct MaxpTable
{
	double TableVersionNumber;
	unsigned short NumGlyphs;
	unsigned short MaxPoints;
	unsigned short MaxCountours;
	unsigned short MaxCompositePoints;
	unsigned short MaxCompositeContours;
	unsigned short MaxZones;
	unsigned short MaxTwilightPoints;
	unsigned short MaxStorage;
	unsigned short MaxFunctionDefs;
	unsigned short MaxInstructionDefs;
	unsigned short MaxStackElements;
	unsigned short MaxSizeOfInstructions;
	unsigned short MaxComponentElements;
	unsigned short MaxCompontentDepth;
};

struct HHeaTable
{
	double TableVersionNumber;
	short Ascender;
	short Descender;
	short LineGap;
	unsigned short AdvanceWidthMax;
	short MinLeftSideBearing;
	short MinRightSideBearing;
	short XMaxExtent;
	short CaretSlopeRise;
	short CaretSlopeRun;
	short CaretOffset;
	short MetricDataFormat;
	unsigned short NumberOfHMetrics;
};

struct HMtxTableEntry
{
	unsigned short AdvanceWidth;
	short LeftSideBearing;
};

typedef HMtxTableEntry* HMtxTable;

struct OS2Table
{
	unsigned short Version;
	short AvgCharWidth;
	unsigned short WeightClass;
	unsigned short WidthClass;
	unsigned short fsType;
	short SubscriptXSize;
	short SubscriptYSize;
	short SubscriptXOffset;
	short SubscriptYOffset;
	short SuperscriptXSize;
	short SuperscriptYSize;
	short SuperscriptXOffset;
	short SuperscriptYOffset;
	short StrikeoutSize;
	short StrikeoutPosition;
	short FamilyClass;
	Byte Panose[10];
	unsigned long UnicodeRange1;
	unsigned long UnicodeRange2;
	unsigned long UnicodeRange3;
	unsigned long UnicodeRange4;
	char AchVendID[4];
	unsigned short FSSelection;
	unsigned short FirstCharIndex;
	unsigned short LastCharIndex;
	short TypoAscender;
	short TypoDescender;
	short TypoLineGap;
	unsigned short WinAscent;
	unsigned short WinDescent;
	unsigned long CodePageRange1;
	unsigned long CodePageRange2;
	short XHeight;
	short CapHeight;
	unsigned short DefaultChar;
	unsigned short BreakChar;
	unsigned short MaxContext;
};

struct NameTableEntry
{
	unsigned short PlatformID;
	unsigned short EncodingID;
	unsigned short LanguageID;
	unsigned short NameID;
	unsigned short Length;
	unsigned short Offset;
	char* String;
};


struct NameTable
{
	unsigned short mNameEntriesCount;
	NameTableEntry* mNameEntries;
};

typedef unsigned long* LocaTable;
typedef list<unsigned long> ULongList;

/* this time it's gonna be just what's intersting for my subsetting purposes - which is the 
   dependencies ('n some other stuff) */
struct GlyphEntry
{
	short NumberOfContours;
	short XMin;
	short YMin;
	short XMax;
	short YMax;
	ULongList mComponentGlyphs; // will be empty for simple glyphs, and with component glyph indexes for components
};

typedef GlyphEntry** GlyfTable;

typedef map<unsigned short,GlyphEntry*> UShortToGlyphEntryMap;


enum EOpenTypeInputType
{
	EOpenTypeTrueType,
	EOpenTypeCFF
};

class OpenTypeFileInput
{
public:
	OpenTypeFileInput(void);
	~OpenTypeFileInput(void);


	EStatusCode ReadOpenTypeFile(const wstring& inFontFilePath);
	EStatusCode ReadOpenTypeFile(IByteReaderWithPosition* inTrueTypeFile);


	EOpenTypeInputType GetOpenTypeFontType();

	unsigned short GetGlyphsCount();

	// read tables
	HeadTable mHead;
	MaxpTable mMaxp;
	HHeaTable mHHea;
	HMtxTable mHMtx;
	OS2Table mOS2;
	NameTable mName;
	LocaTable mLoca;
	GlyfTable mGlyf;

	// not read, but can tell if they are there
	bool mCVTExists;
	bool mFPGMExists;
	bool mPREPExists;

	TableEntry* GetTableEntry(const char* inTagName);

	CFFFileInput mCFF;
	
private:
	OpenTypePrimitiveReader mPrimitivesReader;
	EOpenTypeInputType mFontType;
	unsigned short mTablesCount;
	ULongToTableEntryMap mTables;
	UShortToGlyphEntryMap mActualGlyphs; // using actual glyphs to map the glyphs that are not empty 
										 // (yeah, when parsing subset fonts...some glyphs might just
										 // be empty, to avoid having to change the glyphs indices. some
										 // technique some producers use

	EStatusCode ReadOpenTypeHeader();
	EStatusCode ReadOpenTypeSFNT();
	EStatusCode ReadHead();
	EStatusCode ReadMaxP();
	EStatusCode ReadHHea();
	EStatusCode ReadHMtx();
	EStatusCode ReadOS2();
	EStatusCode ReadName();
	EStatusCode ReadLoca();
	EStatusCode ReadGlyfForDependencies();
	unsigned long GetTag(const char* inTagName);
	void FreeTables();
	EStatusCode ReadCFF();

};

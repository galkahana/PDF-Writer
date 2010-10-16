#pragma once
/*	
	Hey, initially this is not a full fledged PFM file reader.
	i'm just reading the bare minimum to get some required type1 font
	values
*/

#include "EStatusCode.h"
#include "InputFile.h"


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

struct PFMHeader
{
	WORD Version;
	DWORD Size;
	BYTE Copyright[60];
	WORD Type;
	WORD Point;
	WORD VertRes;
	WORD HorizRes;
	WORD Ascent;
	WORD InternalLeading;
	WORD ExternalLeading;
	BYTE Italic;
	BYTE Underline;
	BYTE StrikeOut;
	WORD Weight;
	BYTE CharSet;
	WORD PixWidth;
	WORD PixHeight;
	BYTE PitchAndFamily;
	WORD AvgWidth;
	WORD MaxWidth;
	BYTE FirstChar;
	BYTE LastChar;
	Byte DefaultChar;
	Byte BreakChar;
	WORD WidthBytes;
	DWORD Device;
	DWORD Face;
	DWORD BitsPinter;
	DWORD BitsOffset;

};

struct PFMExtension
{
	WORD SizeFields;
	DWORD ExtMetricsOffset;
	DWORD ExtentTable;
	DWORD OriginTable;
	DWORD PairKernTable;
	DWORD TrackKernTable;
	DWORD DriverInfo;
	DWORD Reserved;
};

struct PFMExtendedFontMetrics
{
	WORD Size;
	WORD PointSize;
	WORD Orientation;
	WORD MasterHeight;
	WORD MinScale;
	WORD MaxScale;
	WORD MasterUnits;
	WORD CapHeight;
	WORD XHeight;
	WORD LowerCaseAscent;
	WORD LowerCaseDescent;
	WORD Slant;
	WORD SuperScript;
	WORD SubScript;
	WORD UnderlineOffset;
	WORD UnderlineWidth;
	WORD DoubleUpperUnderlineOffset;
	WORD DoubleLowerUnderlineOffset;
	WORD DoubleUpperUnderlineWidth;
	WORD DoubleLowerUnderlineWidth;
	WORD StrikeOutOffset;
	WORD StrikeOutWidth;
	WORD KernPairs;
	WORD KernTracks;
};

class IByteReader;

class PFMFileReader
{
public:
	PFMFileReader(void);
	~PFMFileReader(void);

	EStatusCode Read(const wstring& inPFMFilePath);

	PFMHeader Header;
	PFMExtension Extension;
	PFMExtendedFontMetrics ExtendedFontMetrics;

	// as i said, read just what i need

private:

	IByteReader* mReaderStream;
	EStatusCode mInternalReadStatus;

	EStatusCode ReadByte(BYTE& outByte);
	EStatusCode ReadWord(WORD& outWORD);
	EStatusCode ReadDWord(DWORD& outDWORD);

	EStatusCode ReadHeader();
	EStatusCode ReadExtension();
	EStatusCode ReadExtendedFontMetrics();
};

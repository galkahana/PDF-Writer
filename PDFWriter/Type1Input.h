#pragma once
#include "EStatusCode.h"
#include "InputPFBDecodeStream.h"
#include "IType1InterpreterImplementation.h"

#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

enum EType1EncodingType
{
	eType1EncodingTypeStandardEncoding,
	eType1EncodingTypeCustom
};

struct Type1Encoding
{
	EType1EncodingType EncodingType;
	string mCustomEncoding[256];
};

struct Type1FontDictionary
{
	string FontName;
	int PaintType;
	int FontType;
	double FontMatrix[6];
	double FontBBox[4];
	int UniqueID;
	// Metrics ignored
	double StrokeWidth;
};

struct Type1FontInfoDictionary
{
	string version;
	string Notice;
	string Copyright;
	string FullName;
	string FamilyName;
	string Weight;
	double ItalicAngle;
	bool isFixedPitch;
	double UnderlinePosition;
	double UnderlineThickness;
};

struct Type1PrivateDictionary
{
	int UniqueID;
	vector<int> BlueValues;
	vector<int> OtherBlues;
	vector<int> FamilyBlues;
	vector<int> FamilyOtherBlues;
	double BlueScale;
	int BlueShift;
	int BlueFuzz;
	double StdHW;
	double StdVW;
	vector<double> StemSnapH;
	vector<double> StemSnapV;
	bool ForceBold;
	int LanguageGroup;
	int lenIV;
	bool RndStemUp;
};


typedef set<Byte> ByteSet;
typedef set<unsigned short> UShortSet;

struct CharString1Dependencies
{
	ByteSet mCharCodes; // from seac operator
	UShortSet mOtherSubrs; // from callothersubr
	UShortSet mSubrs; // from callsubr
};


typedef vector<Type1CharString> Type1CharStringVector;
typedef map<string,Type1CharString> StringToType1CharStringMap;

class IByteReaderWithPosition;

class Type1Input : public Type1InterpreterImplementationAdapter
{
public:
	Type1Input(void);
	~Type1Input(void);

	EStatusCode ReadType1File(IByteReaderWithPosition* inType1File);
	EStatusCode CalculateDependenciesForCharIndex(Byte inCharStringIndex,
												  CharString1Dependencies& ioDependenciesInfo);
	EStatusCode CalculateDependenciesForCharIndex(const string& inCharStringName,
												  CharString1Dependencies& ioDependenciesInfo);
	void Reset();
	Type1CharString* GetGlyphCharString(const string& inCharStringName);
	Type1CharString* GetGlyphCharString(Byte inCharStringIndex);
	string GetGlyphCharStringName(Byte inCharStringIndex);
	bool IsValidGlyphIndex(Byte inCharStringIndex);
	
	// some structs for you all laddies and lasses
	Type1FontDictionary mFontDictionary;
	Type1FontInfoDictionary mFontInfoDictionary;
	Type1PrivateDictionary mPrivateDictionary;


	// IType1InterpreterImplementation overrides
	virtual Type1CharString* GetSubr(long inSubrIndex);
	virtual EStatusCode Type1Seac(const LongList& inOperandList);
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
	virtual unsigned long GetLenIV();

private:
	Type1Encoding mEncoding;
	Byte mSubrsCount;
	Type1CharString* mSubrs;
	StringToType1CharStringMap mCharStrings;

	InputPFBDecodeStream mPFBDecoder;

	CharString1Dependencies* mCurrentDependencies;


	void FreeTables();
	bool IsComment(const string& inToken);
	EStatusCode ReadFontDictionary();
	EStatusCode ReadFontInfoDictionary();
	string FromPSName(const string& inPostScriptName);
	EStatusCode ParseEncoding();
	EStatusCode ReadPrivateDictionary();
	EStatusCode ParseIntVector(vector<int>& inVector);
	EStatusCode ParseDoubleVector(vector<double>& inVector);
	EStatusCode ParseSubrs();
	EStatusCode ParseCharstrings();
	EStatusCode ParseDoubleArray(double* inArray,int inArraySize);
};

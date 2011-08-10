/*
   Source File : Type1Input.h


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
#pragma once
#include "EPDFStatusCode.h"
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
	bool FSTypeValid;
	unsigned short fsType;
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
	bool FSTypeValid;
	unsigned short fsType;
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
typedef map<string,Byte> StringToByteMap;

class IByteReaderWithPosition;

class Type1Input : public Type1InterpreterImplementationAdapter
{
public:
	Type1Input(void);
	~Type1Input(void);

	EPDFStatusCode ReadType1File(IByteReaderWithPosition* inType1File);
	EPDFStatusCode CalculateDependenciesForCharIndex(Byte inCharStringIndex,
												  CharString1Dependencies& ioDependenciesInfo);
	EPDFStatusCode CalculateDependenciesForCharIndex(const string& inCharStringName,
												  CharString1Dependencies& ioDependenciesInfo);
	void Reset();
	Type1CharString* GetGlyphCharString(const string& inCharStringName);
	Type1CharString* GetGlyphCharString(Byte inCharStringIndex);
	string GetGlyphCharStringName(Byte inCharStringIndex);
	bool IsValidGlyphIndex(Byte inCharStringIndex);
	Byte GetEncoding(const string& inCharStringName);
	
	// some structs for you all laddies and lasses
	Type1FontDictionary mFontDictionary;
	Type1FontInfoDictionary mFontInfoDictionary;
	Type1PrivateDictionary mPrivateDictionary;

	// IType1InterpreterImplementation overrides
	virtual Type1CharString* GetSubr(long inSubrIndex);
	virtual EPDFStatusCode Type1Seac(const LongList& inOperandList);
	virtual bool IsOtherSubrSupported(long inOtherSubrsIndex);
	virtual unsigned long GetLenIV();

private:
	Type1Encoding mEncoding;
	StringToByteMap mReverseEncoding;
	Byte mSubrsCount;
	Type1CharString* mSubrs;
	StringToType1CharStringMap mCharStrings;

	InputPFBDecodeStream mPFBDecoder;

	CharString1Dependencies* mCurrentDependencies;


	void FreeTables();
	bool IsComment(const string& inToken);
	EPDFStatusCode ReadFontDictionary();
	EPDFStatusCode ReadFontInfoDictionary();
	string FromPSName(const string& inPostScriptName);
	EPDFStatusCode ParseEncoding();
	EPDFStatusCode ReadPrivateDictionary();
	EPDFStatusCode ParseIntVector(vector<int>& inVector);
	EPDFStatusCode ParseDoubleVector(vector<double>& inVector);
	EPDFStatusCode ParseSubrs();
	EPDFStatusCode ParseCharstrings();
	EPDFStatusCode ParseDoubleArray(double* inArray,int inArraySize);
	string FromPSString(const string& inPSString);
	void CalculateReverseEncoding();
};

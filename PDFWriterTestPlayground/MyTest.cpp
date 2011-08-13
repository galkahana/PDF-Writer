/*
   Source File : MyTest.cpp


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
#include "MyTest.h"
#include "TestsRunner.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <iostream>

using namespace PDFHummus;

#define N_STD_STRINGS 391
static const char* scStandardStrings[N_STD_STRINGS] = {
		".notdef","space","exclam","quotedbl","numbersign","dollar","percent","ampersand","quoteright","parenleft",
		"parenright","asterisk","plus","comma","hyphen","period","slash","zero","one","two",
		"three","four","five","six","seven","eight","nine","colon","semicolon","less",
		"equal","greater","question","at","A","B","C","D","E","F",
		"G","H","I","J","K","","M","N","O","P",
		"Q","R","S","T","U","V","W","X","Y","Z",
		"bracketleft","backslash","bracketright","asciicircum","underscore","quoteleft","a","b","c","d",
		"e","f","g","h","i","j","k","l","m","n",
		"o","p","q","r","s","t","u","v","w","x",
		"y","z","braceleft","bar","braceright","asciitilde","exclamdown","cent","sterling","fraction",
		"yen","florin","section","currency","quotesingle","quotedblleft","guillemotleft","guilsinglleft","guilsinglright","fi",
		"fl","endash","dagger","daggerdbl","periodcentered","paragraph","bullet","quotesinglbase","quotedblbase","quotedblright",
		"guillemotright","ellipsis","perthousand","questiondown","grave","acute","circumflex","tilde","macron","breve",
		"dotaccent","dieresis","ring","cedilla","hungarumlaut","ogonek","caron","emdash","AE","ordfeminine",
		"Lslash","Oslash","OE","ordmasculine","ae","dotlessi","lslash","oslash","oe","germandbls",
		"onesuperior","logicalnot","mu","trademark","Eth","onehalf","plusminus","Thorn","onequarter","divide",
		"brokenbar","degree","thorn","threequarters","twosuperior","registered","minus","eth","multiply","threesuperior",
		"copyright","Aacute","Acircumflex","Adieresis","Agrave","Aring","Atilde","Ccedilla","Eacute","Ecircumflex",
		"Edieresis","Egrave","Iacute","Icircumflex","Idieresis","Igrave","Ntilde","Oacute","Ocircumflex","Odieresis",
		"Ograve","Otilde","Scaron","Uacute","Ucircumflex","Udieresis","Ugrave","Yacute","Ydieresis","Zcaron",
		"aacute","acircumflex","adieresis","agrave","aring","atilde","ccedilla","eacute","ecircumflex","edieresis",
		"egrave","iacute","icircumflex","idieresis","igrave","ntilde","oacute","ocircumflex","odieresis","ograve",
        "otilde","scaron","uacute","ucircumflex","udieresis","ugrave","yacute","ydieresis","zcaron","exclamsmall",
		"Hungarumlautsmall","dollaroldstyle","dollarsuperior","ampersandsmall","Acutesmall","parenleftsuperior","parenrightsuperior","twodotenleader","onedotenleader","zerooldstyle",
		"oneoldstyle","twooldstyle","threeoldstyle","fouroldstyle","fiveoldstyle","sixoldstyle","sevenoldstyle","eightoldstyle","nineoldstyle","commasuperior",
		"threequartersemdash","periodsuperior","questionsmall","asuperior","bsuperior","centsuperior","dsuperior","esuperior","isuperior","lsuperior",
		"msuperior","nsuperior","osuperior","rsuperior","ssuperior","tsuperior","ff","ffi","ffl","parenleftinferior",
		"parenrightinferior","Circumflexsmall","hyphensuperior","Gravesmall","Asmall","Bsmall","Csmall","Dsmall","Esmall","Fsmall",
		"Gsmall","Hsmall","Ismall","Jsmall","Ksmall","Lsmall","Msmall","Nsmall","Osmall","Psmall",
		"Qsmall","Rsmall","Ssmall","Tsmall","Usmall","Vsmall","Wsmall","Xsmall","Ysmall","Zsmall",
		"colonmonetary","onefitted","rupiah","Tildesmall","exclamdownsmall","centoldstyle","Lslashsmall","Scaronsmall","Zcaronsmall","Dieresissmall",
		"Brevesmall","Caronsmall","Dotaccentsmall","Macronsmall","figuredash","hypheninferior","Ogoneksmall","Ringsmall","Cedillasmall","questiondownsmall",
		"oneeighth","threeeighths","fiveeighths","seveneighths","onethird","twothirds","zerosuperior","foursuperior","fivesuperior","sixsuperior",
		"sevensuperior","eightsuperior","ninesuperior","zeroinferior","oneinferior","twoinferior","threeinferior","fourinferior","fiveinferior","sixinferior",
		"seveninferior","eightinferior","nineinferior","centinferior","dollarinferior","periodinferior","commainferior","Agravesmall","Aacutesmall","Acircumflexsmall",
		"Atildesmall","Adieresissmall","Aringsmall","AEsmall","Ccedillasmall","Egravesmall","Eacutesmall","Ecircumflexsmall","Edieresissmall","Igravesmall",
		"Iacutesmall","Icircumflexsmall","Idieresissmall","Ethsmall","Ntildesmall","Ogravesmall","Oacutesmall","Ocircumflexsmall","Otildesmall","Odieresissmall",
		"OEsmall","Oslashsmall","Ugravesmall","Uacutesmall","Ucircumflexsmall","Udieresissmall","Yacutesmall","Thornsmall","Ydieresissmall","001.000",
		"001.001","001.002","001.003","Black","Bold","Book","Light","Medium","Regular","Roman",
		"Semibold"
};

MyTest::MyTest(void)
{
}

MyTest::~MyTest(void)
{
}

using namespace std;

typedef pair<const char*,int> StringAndInt;
typedef vector<StringAndInt> StringAndIntVector;

static bool sStringSort(const StringAndInt& inLeft, const StringAndInt& inRight)
{
	return strcmp(inLeft.first,inRight.first) < 0;
}

EStatusCode MyTest::Run()
{
	StringAndIntVector stringsVector;
	int i=0;

	for(;i<N_STD_STRINGS;++i)
		stringsVector.push_back(StringAndInt(scStandardStrings[i],i));

	sort(stringsVector.begin(),stringsVector.end(),sStringSort);

	cout<<"Sorted Strings Vector:\n";

	cout<<"static const char* scSortedStandardStrings[N_STD_STRINGS] = {\n";
	int j=0;

	for(i=0;i<N_STD_STRINGS-1;++i)
	{
		cout<<"\""<<stringsVector[i].first<<"\",";
		if(9 == j)
		{
			cout<<"\n";
			j = 0;
		}
		else
			++j;
	}
	cout<<"\""<<stringsVector[N_STD_STRINGS-1].first<<"\"\n};\n";

	cout<<"Sorted Strings Vector positions:\n";
	cout<<"static int scSortedStandardStringsPositions[N_STD_STRINGS] = {\n";
	j=0;

	for(i=0;i<N_STD_STRINGS-1;++i)
	{
		cout<<stringsVector[i].second<<",";
		if(9 == j)
		{
			cout<<"\n";
			j = 0;
		}
		else
			++j;
	}	
	cout<<stringsVector[N_STD_STRINGS-1].second<<"\n};\n";

	return PDFHummus::eSuccess;
}


ADD_CATEGORIZED_TEST(MyTest,"MyTest")

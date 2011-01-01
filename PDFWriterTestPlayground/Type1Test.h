#pragma once
#include "ITestUnit.h"

#include <string>
#include <vector>

struct Type1FontDictionary;
struct Type1PrivateDictionary;
struct Type1FontInfoDictionary;

using namespace std;

class Type1Input;

class Type1Test : public ITestUnit
{
public:
	Type1Test(void);
	~Type1Test(void);

	virtual EStatusCode Run();

private:
	EStatusCode SaveCharstringCode(const string& inCharStringName,Type1Input* inType1Input);
	EStatusCode ShowDependencies(const string& inCharStringName,Type1Input* inType1Input);

	void ShowFontDictionary(const Type1FontDictionary& inFontDictionary);
	void ShowFontInfoDictionary(const Type1FontInfoDictionary& inFontInfoDictionary);
	void ShowPrivateInfoDictionary(const Type1PrivateDictionary& inPrivateDictionary);
	void ShowDoubleArray(const double inDoubleArray[],int inSize);
	void ShowIntVector(const vector<int>& inVector);
	void ShowDoubleVector(const vector<double>& inVector);

};

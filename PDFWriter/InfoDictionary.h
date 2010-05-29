#pragma once

#include "PDFTextString.h"
#include "PDFDate.h"
#include "MapIterator.h"
#include <string>
#include <map>

using namespace std;

typedef std::map<string,PDFTextString> StringToPDFTextString;

enum EInfoTrapped
{
	EInfoTrappedTrue,
	EInfoTrappedFalse,
	EInfoTrappedUnknown
};

class InfoDictionary
{
public:
	InfoDictionary(void);
	~InfoDictionary(void);

	PDFTextString Title;
	PDFTextString Author;
	PDFTextString Subject;
	PDFTextString Keywords;
	PDFTextString Creator;
	PDFTextString Producer;
	PDFDate CreationDate;
	PDFDate ModDate;
	EInfoTrapped Trapped;

	void AddAdditionalInfoEntry(const string& inKey,
								const PDFTextString& inValue);
	void RemoveAdditionalInfoEntry(const string& inKey);
	PDFTextString GetAdditionalInfoEntry(const string& inKey);

	MapIterator<StringToPDFTextString> GetAdditionaEntriesIterator();


	bool IsEmpty();
private:

	StringToPDFTextString mAdditionalInfoEntries;
};


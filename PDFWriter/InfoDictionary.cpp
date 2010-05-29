#include "InfoDictionary.h"

InfoDictionary::InfoDictionary(void)
{
	Trapped = EInfoTrappedUnknown;
}

InfoDictionary::~InfoDictionary(void)
{
}

bool InfoDictionary::IsEmpty()
{
	return 
		Title.IsEmpty() &&
		Author.IsEmpty() &&
		Subject.IsEmpty() &&
		Keywords.IsEmpty() &&
		Creator.IsEmpty() &&
		Producer.IsEmpty() &&
		CreationDate.IsNull() &&
		ModDate.IsNull() &&
		EInfoTrappedUnknown == Trapped &&
		mAdditionalInfoEntries.empty();

}

void InfoDictionary::AddAdditionalInfoEntry(const string& inKey,
											const PDFTextString& inValue)
{
	if(mAdditionalInfoEntries.find(inKey) != mAdditionalInfoEntries.end())
		mAdditionalInfoEntries[inKey] = inValue;
	else
		mAdditionalInfoEntries.insert(StringToPDFTextString::value_type(inKey,inValue));
}

void InfoDictionary::RemoveAdditionalInfoEntry(const string& inKey)
{
	StringToPDFTextString::iterator it = mAdditionalInfoEntries.find(inKey);
	if(it != mAdditionalInfoEntries.end())
		mAdditionalInfoEntries.erase(it);
}

PDFTextString InfoDictionary::GetAdditionalInfoEntry(const string& inKey)
{
	StringToPDFTextString::iterator it = mAdditionalInfoEntries.find(inKey);

	if(it == mAdditionalInfoEntries.end())
		return PDFTextString::Empty;
	else
		return it->second;
}

MapIterator<StringToPDFTextString> InfoDictionary::GetAdditionaEntriesIterator()
{
	return MapIterator<StringToPDFTextString>(mAdditionalInfoEntries);
}

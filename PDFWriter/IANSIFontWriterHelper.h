#pragma once

class DictionaryContext;
class IFontDescriptorHelper;

class IANSIFontWriterHelper
{
public:
	virtual ~IANSIFontWriterHelper(){}

	// write the sub type value (only, key is already written at this point) for the particular font into the dictionary.
	virtual void WriteSubTypeValue(DictionaryContext* inDictionary) = 0;

	// Get the relevant CharSet entry writer for the descriptor writing. 
	// always return something - even if doing nothing
	virtual IFontDescriptorHelper* GetCharsetWriter() = 0;
};
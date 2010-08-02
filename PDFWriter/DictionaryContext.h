#pragma once
#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"

#include <string>
#include <set>

using namespace std;

typedef set<string> StringSet;

class ObjectsContext;

class DictionaryContext
{
public:
	DictionaryContext(ObjectsContext* inObjectsContext,size_t inIndentLevel);
	~DictionaryContext(void);

	EStatusCode WriteKey(const string& inKey);
	
	void WriteIntegerValue(long long inValue);
	void WriteLiteralStringValue(const string& inValue);
	void WriteNameValue(const string& inValue);
	void WriteObjectReferenceValue(ObjectIDType inObjectReference);
	void WriteRectangleValue(const PDFRectangle& inRectangle);
	void WriteDoubleValue(double inValue);
	void WriteBooleanValue(bool inValue);

private:

	ObjectsContext* mObjectsContext;
	StringSet mKeys;
	size_t mIndentLevel;

	void WriteIndents();

};

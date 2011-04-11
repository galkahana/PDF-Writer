#pragma once

#include "TestsRunner.h"
#include "ObjectsBasicTypes.h"


#include <set>

class PDFObject;
class PDFParser;
class IByteWriter;

typedef set<ObjectIDType> ObjectIDTypeSet;

class PDFParserTest : public ITestUnit
{
public:
	PDFParserTest(void);
	virtual ~PDFParserTest(void);

	virtual EStatusCode Run();

private:

	EStatusCode IterateObjectTypes(PDFObject* inObject,PDFParser& inParser,IByteWriter* inOutput);

	int mTabLevel;
	ObjectIDTypeSet mIteratedObjectIDs;
};

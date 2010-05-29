#pragma once

#include "SingleValueContainerIterator.h"

#include <set>
#include <string>

using namespace std;

// Predefined procset names
static const string KProcsetPDF = "PDF";
static const string KProcsetText = "Text";
static const string KProcsetImageB = "ImageB";
static const string KProcsetImageC = "ImageC";
static const string KProcsetImageI = "ImageI";


typedef set<string> StringSet;

class ResourcesDictionary
{
public:
	ResourcesDictionary(void);
	virtual ~ResourcesDictionary(void);

	void AddProcsetResource(const string& inResourceName);
	int GetProcsetsCount();
	SingleValueContainerIterator<StringSet> GetProcesetsIterator();

private:

	StringSet mProcsets;
	
};


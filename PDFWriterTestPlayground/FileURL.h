#pragma once

#include <string>
#include <list>

#include "SingleValueContainerIterator.h"

using namespace std;

typedef list<string> StringList;

class FileURL
{
public:
	FileURL(void);
	FileURL(const string& inURL);
	~FileURL(void);
    
	void SetURLFromString(const string& inURL);
    
    bool IsAbsolute() const;
	FileURL GetFolder() const;
	FileURL InterpretFrom(const FileURL& inURL) const;
    
	string ToString() const;
    
    
	void PushPathComponent(const string& inComponent);
	void PopPathComponent();
	void SetIsAbsolute(bool inIsAbsolute);
    
    SingleValueContainerIterator<const StringList> GetComponentsIterator() const;
    
private:
    
	bool mIsAbsolute;
	StringList mPathComponents;
};
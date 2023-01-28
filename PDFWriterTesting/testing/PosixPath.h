#pragma once

#include "FileURL.h"

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class PosixPath
{
public:
	PosixPath(void);
	PosixPath(const string& inPath);
	~PosixPath(void);
    
	void SetPathFromString(const string& inPath);
    
    bool IsAbsolute() const;
	PosixPath GetFolder() const;
	PosixPath InterpretFrom(const PosixPath& inPath) const;
    
	string ToString() const;
    
    FileURL ToFileURL() const;
    void FromFileURL(const FileURL& inFileURL);
    
	void PushPathComponent(const string& inComponent);
	void PopPathComponent();
	void SetIsAbsolute(bool inIsAbsolute);
private:
    
	bool mIsAbsolute;
	StringList mPathComponents;
};
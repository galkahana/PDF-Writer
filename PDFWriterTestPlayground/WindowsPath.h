#pragma once

#include "FileURL.h"

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class WindowsPath
{
public:
	WindowsPath(void);
	WindowsPath(const string& inPath);
	~WindowsPath(void);
    
	void SetPathFromString(const string& inPath);
    
	bool IsAbsolute() const;
	WindowsPath GetFolder() const;
	WindowsPath InterpretFrom(const WindowsPath& inPath) const;
    
	string ToString() const;
    
    
    FileURL ToFileURL() const;
    void FromFileURL(const FileURL& inFileURL);
    
	void PushPathComponent(const string& inComponent);
	void PopPathComponent();
	void SetDriveLetter(const string& inDriveLetter);
private:
    
	string mDriveLetter;
	StringList mPathComponents;
};
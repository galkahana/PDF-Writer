#include "WindowsPath.h"

#include <sstream>

WindowsPath::WindowsPath(void)
{
}

WindowsPath::WindowsPath(const string& inPath)
{
	SetPathFromString(inPath);
}

WindowsPath::~WindowsPath(void)
{
}

void WindowsPath::SetPathFromString(const string& inPath)
{
	// path is expected to be either <driveLetter>:\<folder1>\<folder2>....
	// or <folder1>\<folder2>...
    
	mPathComponents.clear();
	mDriveLetter = "";	 
    
	string::size_type searchPosition = 0;
    
	if(inPath.size() > 1 && inPath.at(1) == ':')
	{
		mDriveLetter = inPath.substr(0,1);
		searchPosition = 3; // may be eitehr c: or c:\...., which should be equivelent
	}
    
	string newComponent;
	while(searchPosition < inPath.length())
	{
		string::size_type findResult = inPath.find("\\",searchPosition);
		if(findResult == inPath.npos)
			newComponent = inPath.substr(searchPosition,findResult);
		else
			newComponent = inPath.substr(searchPosition,findResult-searchPosition);
        
		// with .. i'm doing some calculation already of sparing interim folders.
		// as a result if there are still ..s they will be only at the beginning of the path
		if(newComponent == ".." && mPathComponents.size() > 0 && mPathComponents.back() != "..")
			PopPathComponent();
		else if(newComponent != ".")
			PushPathComponent(newComponent);
        
		searchPosition = (findResult == inPath.npos) ? inPath.length() : findResult+1;
	}
}

bool WindowsPath::IsAbsolute() const
{
	return mDriveLetter.length() > 0;
}

WindowsPath WindowsPath::GetFolder() const
{
	WindowsPath newPath;
    
	// in edge case of drive letter only, will return the same path. in empty relative, will enter empty relative
	// in all other cases will return a path which is the same drive letter, and with one less component from the component list
    
	newPath.SetDriveLetter(mDriveLetter);
	newPath.mPathComponents = mPathComponents;
	newPath.PopPathComponent();
	return newPath;
}

WindowsPath WindowsPath::InterpretFrom(const WindowsPath& inPath) const
{
	if(IsAbsolute()) // for absolute paths retain as is
		return *this;
    
	// otherwise, construct a new path from the two paths, and return
    
	WindowsPath newPath = inPath;
    
	StringList::const_iterator it = mPathComponents.begin();
	for(; it != mPathComponents.end();++it)
	{
		if(*it == "..")
			newPath.PopPathComponent();
		else
			newPath.PushPathComponent(*it);
	}
    
	return newPath;
}

string WindowsPath::ToString() const
{
	stringstream pathStream;
    
	if(mDriveLetter != "")
		pathStream<<mDriveLetter<<":\\";
    
	if(mPathComponents.size() > 0)
	{
		StringList::const_iterator it = mPathComponents.begin();
		pathStream<<*it;
		++it;
        
		for(; it != mPathComponents.end();++it)
			pathStream<<"\\"<<*it;
	}
    
	return pathStream.str();
}

void WindowsPath::PushPathComponent(const string& inComponent)
{
	mPathComponents.push_back(inComponent);
}

void WindowsPath::PopPathComponent()
{
	if(mPathComponents.size() > 0)
		mPathComponents.pop_back();
}

void WindowsPath::SetDriveLetter(const string& inDriveLetter)
{
	mDriveLetter = inDriveLetter;
}

FileURL WindowsPath::ToFileURL() const
{
    FileURL aFileURL;
    
    if(IsAbsolute())
    {
        // a drive letter is written to path like this: file:// /c|
        aFileURL.PushPathComponent(mDriveLetter + "|");
        aFileURL.SetIsAbsolute(true);
   }
    
    StringList::const_iterator it = mPathComponents.begin();
    
    for(; it != mPathComponents.end(); ++it)
        aFileURL.PushPathComponent(*it);
    
    return aFileURL;
}

void WindowsPath::FromFileURL(const FileURL& inFileURL)
{
    SingleValueContainerIterator<const StringList> itComponents = inFileURL.GetComponentsIterator();

 	mPathComponents.clear();
	mDriveLetter = "";	   
    
    // move to first component (if empty...just return)
    if(!itComponents.MoveNext())
        return;
    
    // check first component, if it's a drive letter
    if(itComponents.GetItem().at(itComponents.GetItem().length() - 1) == '|')
        mDriveLetter = itComponents.GetItem().substr(0,itComponents.GetItem().length() - 1);
    else
        mPathComponents.push_back(itComponents.GetItem());
    
    while(itComponents.MoveNext())
        mPathComponents.push_back(itComponents.GetItem());
        
}

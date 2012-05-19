#include "PosixPath.h"

#include <sstream>

PosixPath::PosixPath(void)
{
    mIsAbsolute = false;
}

PosixPath::PosixPath(const string& inPath)
{
	SetPathFromString(inPath);
}

PosixPath::~PosixPath(void)
{
}



void PosixPath::SetPathFromString(const string& inPath)
{
	// path is expected to be either /<folder1>/<folder2>....
	// or <folder1>/<folder2>...
    
	mPathComponents.clear();
	mIsAbsolute = false;	 
    
	string::size_type searchPosition = 0;
    
	if(inPath.size() > 1 && (inPath.at(0) == '/'))
	{
        mIsAbsolute = true;
		searchPosition = 1; 
	}
    
	string newComponent;
	while(searchPosition < inPath.length())
	{
		string::size_type findResult = inPath.find("/",searchPosition);
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

bool PosixPath::IsAbsolute() const
{
	return mIsAbsolute;
}

PosixPath PosixPath::GetFolder() const
{
	PosixPath newPath;
    
	// in edge case of "/" , will return the same path. in empty relative, will enter empty relative
	// in all other cases will return a url with one less component from the component list
    
	newPath.SetIsAbsolute(mIsAbsolute);
	newPath.mPathComponents = mPathComponents;
	newPath.PopPathComponent();
	return newPath;
}

PosixPath PosixPath::InterpretFrom(const PosixPath& inURL) const
{
	if(IsAbsolute()) // for absolute paths retain as is
		return *this;
    
	// otherwise, construct a new path from the two paths, and return
    
	PosixPath newURL = inURL;
    
	StringList::const_iterator it = mPathComponents.begin();
	for(; it != mPathComponents.end();++it)
	{
		if(*it == "..")
			newURL.PopPathComponent();
		else
			newURL.PushPathComponent(*it);
	}
    
	return newURL;
}

string PosixPath::ToString() const
{
	stringstream pathStream;
    
	if(mIsAbsolute)
		pathStream<<"/";
    
	if(mPathComponents.size() > 0)
	{
		StringList::const_iterator it = mPathComponents.begin();
		pathStream<<*it;
		++it;
        
		for(; it != mPathComponents.end();++it)
			pathStream<<"/"<<*it;
	}
    
	return pathStream.str();
}

void PosixPath::PushPathComponent(const string& inComponent)
{
	mPathComponents.push_back(inComponent);
}

void PosixPath::PopPathComponent()
{
	if(mPathComponents.size() > 0)
		mPathComponents.pop_back();
}

void PosixPath::SetIsAbsolute(bool inIsAbsolute)
{
	mIsAbsolute = inIsAbsolute;
}

FileURL PosixPath::ToFileURL() const
{
    FileURL aFileURL;
    
    aFileURL.SetIsAbsolute(IsAbsolute());
    
    StringList::const_iterator it = mPathComponents.begin();
    
    for(; it != mPathComponents.end(); ++it)
        aFileURL.PushPathComponent(*it);
    
    return aFileURL;    
}

void PosixPath::FromFileURL(const FileURL& inFileURL)
{
    SingleValueContainerIterator<const StringList> itComponents = inFileURL.GetComponentsIterator();
    
 	mPathComponents.clear();
	mIsAbsolute = inFileURL.IsAbsolute();   
    
    while(itComponents.MoveNext())
        mPathComponents.push_back(itComponents.GetItem());  
}

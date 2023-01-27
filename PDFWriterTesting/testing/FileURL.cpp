#include "FileURL.h"

#include <sstream>

FileURL::FileURL(void)
{
    mIsAbsolute = false;
}

FileURL::FileURL(const string& inURL)
{
	SetURLFromString(inURL);
}

FileURL::~FileURL(void)
{
}



void FileURL::SetURLFromString(const string& inURL)
{
	// path is expected to be either file:/ / /<folder1>/<folder2>....
	// or <folder1>/<folder2>...
    
	mPathComponents.clear();
	mIsAbsolute = false;	 
    
	string::size_type searchPosition = 0;
    
	if(inURL.size() > 8 && (inURL.substr(0,8) == "file:///"))
	{
        mIsAbsolute = true;
		searchPosition = 8; 
	}
    
	string newComponent;
	while(searchPosition < inURL.length())
	{
		string::size_type findResult = inURL.find("/",searchPosition);
		if(findResult == inURL.npos)
			newComponent = inURL.substr(searchPosition,findResult);
		else
			newComponent = inURL.substr(searchPosition,findResult-searchPosition);
        
		// with .. i'm doing some calculation already of sparing interim folders.
		// as a result if there are still ..s they will be only at the beginning of the path
		if(newComponent == ".." && mPathComponents.size() > 0 && mPathComponents.back() != "..")
			PopPathComponent();
		else if(newComponent != ".")
			PushPathComponent(newComponent);
        
		searchPosition = (findResult == inURL.npos) ? inURL.length() : findResult+1;
	}
}

bool FileURL::IsAbsolute() const
{
	return mIsAbsolute;
}

FileURL FileURL::GetFolder() const
{
	FileURL newURL;
    
	// in edge case of f:///, will return the same path. in empty relative, will enter empty relative
	// in all other cases will return a url with one less component from the component list
    
	newURL.SetIsAbsolute(mIsAbsolute);
	newURL.mPathComponents = mPathComponents;
	newURL.PopPathComponent();
	return newURL;
}

FileURL FileURL::InterpretFrom(const FileURL& inURL) const
{
	if(IsAbsolute()) // for absolute paths retain as is
		return *this;
    
	// otherwise, construct a new path from the two paths, and return
    
	FileURL newURL = inURL;
    
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

string FileURL::ToString() const
{
	stringstream pathStream;
    
	if(mIsAbsolute)
		pathStream<<"file:///";
    
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

void FileURL::PushPathComponent(const string& inComponent)
{
	mPathComponents.push_back(inComponent);
}

void FileURL::PopPathComponent()
{
	if(mPathComponents.size() > 0)
		mPathComponents.pop_back();
}

void FileURL::SetIsAbsolute(bool inIsAbsolute)
{
	mIsAbsolute = inIsAbsolute;
}

SingleValueContainerIterator<const StringList> FileURL::GetComponentsIterator() const
{
    return SingleValueContainerIterator<const StringList>(mPathComponents);
}
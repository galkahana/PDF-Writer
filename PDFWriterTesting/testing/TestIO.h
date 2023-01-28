#pragma once

#include "FileURL.h"

// Helper macro to get a path string from file URL, according to the current OS
#ifdef WIN32

    #include "WindowsPath.h"
    static string FileURLToLocalPath(const FileURL& inFileURL)
    {
        WindowsPath windowsPath;
        windowsPath.FromFileURL(inFileURL);
        return windowsPath.ToString();
    }

    static FileURL LocalPathToFileURL(const string& inLocalPath)
    {
        WindowsPath windowsPath(inLocalPath);
        return windowsPath.ToFileURL();
    }
#else

    #include "PosixPath.h"
    static string FileURLToLocalPath(const FileURL& inFileURL)
    {
        PosixPath posixPath;
        posixPath.FromFileURL(inFileURL);
        return posixPath.ToString();
    }

    static FileURL LocalPathToFileURL(const string& inLocalPath)
    {
        PosixPath posixPath(inLocalPath);
        return posixPath.ToFileURL();
    }
#endif

static string RelativeURLToLocalPath(const FileURL& inFileURL,const string& inRelativeURL)
{
    return FileURLToLocalPath(FileURL(inRelativeURL).InterpretFrom(inFileURL));
}


static string BuildRelativeOutputPath(char* argv[], const string& inRelativeURL)
{
    return RelativeURLToLocalPath(LocalPathToFileURL(argv[2]), inRelativeURL);
}

static string BuildRelativeInputPath(char* argv[], const string& inRelativeURL)
{
    return RelativeURLToLocalPath(LocalPathToFileURL(argv[1]), inRelativeURL);
}
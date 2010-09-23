#include "FreeTypeWrapper.h"
#include "Trace.h"
#include "StringTraits.h"
#include "InputFile.h"
#include "IByteReader.h"

#undef __FTERRORS_H__                                           
#define FT_ERRORDEF( e, v, s )  { e, s },                       
#define FT_ERROR_START_LIST     {                               
#define FT_ERROR_END_LIST       { 0, 0 } };                     
                                                             
static const struct                                                    
{                                                               
int          err_code;                                        
const char*  err_msg;                                         
} ft_errors[] =                                                 
                                                             
#include FT_ERRORS_H                                            

FreeTypeWrapper::FreeTypeWrapper(void)
{
	if(FT_Init_FreeType(&mFreeType))
	{
		TRACE_LOG("FreeTypeWrapper::FreeTypeWrapper, unexpected failure. failed to initialize Free Type");
		mFreeType = NULL;
	}
}

FreeTypeWrapper::~FreeTypeWrapper(void)
{
	if(mFreeType)
		FT_Done_FreeType(mFreeType);
}

FT_Face FreeTypeWrapper::NewFace(const string& inFilePath,FT_Long inFontIndex)
{
	FT_Face face;
	
	FT_Error ftStatus = FT_New_Face(mFreeType,inFilePath.c_str(),inFontIndex,&face);

	if(ftStatus)
	{
		TRACE_LOG2("FreeTypeWrapper::NewFace, unable to load font named %s with index %ld",StringTraits(inFilePath).WidenString().c_str(),inFontIndex);
		TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ft_errors[ftStatus].err_code,StringTraits(ft_errors[ftStatus].err_msg).WidenString().c_str());
		face = NULL;
	}
	return face;
}

FT_Face FreeTypeWrapper::NewFace(const string& inFilePath,const string& inSecondaryFilePath,FT_Long inFontIndex)
{
	FT_Face face = NewFace(inFilePath,inFontIndex);
	if(face)
	{
		FT_Error ftStatus = FT_Attach_File(face,inSecondaryFilePath.c_str());
		if(ftStatus != 0)
		{
			TRACE_LOG1("FreeTypeWrapper::NewFace, unable to load secondary file %s",StringTraits(inSecondaryFilePath).WidenString().c_str());
			TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ft_errors[ftStatus].err_code,StringTraits(ft_errors[ftStatus].err_msg).WidenString().c_str());
			DoneFace(face);
			face = NULL;
		}
	}
	return face;
}


// Couldn't find freetype implementation for NewFace with wide charachter paths. so 
// using my own InputFile class i'm implementing a stream for reading a file via FreeType. Through InputFile i can
// use wide charachter paths.
FT_Face FreeTypeWrapper::NewFace(const wstring& inFilePath,FT_Long inFontIndex)
{
	FT_Face face;
	FT_Open_Args openFaceArguments;

	do
	{
		if(FillOpenFaceArgumentsForWideString(inFilePath,openFaceArguments) != eSuccess)
		{
			face = NULL;
			break;
		}

		FT_Error ftStatus =  FT_Open_Face(mFreeType,&openFaceArguments,inFontIndex,&face);

		if(ftStatus)
		{
			TRACE_LOG2("FreeTypeWrapper::NewFace, unable to load font named %s with index %ld",inFilePath.c_str(),inFontIndex);
			TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ft_errors[ftStatus].err_code,StringTraits(ft_errors[ftStatus].err_msg).WidenString().c_str());
			face = NULL;
		}

	}while(false);

	if(!face)
		CloseOpenFaceArgumentsStream(openFaceArguments);
	return face;
}

EStatusCode FreeTypeWrapper::FillOpenFaceArgumentsForWideString(const wstring& inFilePath, FT_Open_Args& ioArgs)
{
	ioArgs.flags = FT_OPEN_STREAM;
	ioArgs.memory_base = NULL;
	ioArgs.memory_size = 0;
	ioArgs.pathname = NULL;
	ioArgs.driver = NULL;
	ioArgs.num_params = 0;
	ioArgs.params = NULL;
	ioArgs.stream = CreateFTStreamForPath(inFilePath);
	
	if(ioArgs.stream)
	{
		return eSuccess;
	}
	else
	{
		TRACE_LOG1("FreeTypeWrapper::FillOpenFaceArgumentsForWideString, Cannot Open file for reading %s",inFilePath);
		return eFailure;
	}
}

void FreeTypeWrapper::CloseOpenFaceArgumentsStream(FT_Open_Args& ioArgs)
{
	if(ioArgs.stream)
	{
		delete (InputFile*)(ioArgs.stream->descriptor.pointer);
		delete ioArgs.stream;
	}
}

FT_Face FreeTypeWrapper::NewFace(const wstring& inFilePath,const wstring& inSecondaryFilePath,FT_Long inFontIndex)
{
	FT_Open_Args attachStreamArguments;

	FT_Face face = NewFace(inFilePath,inFontIndex);
	if(face)
	{
		do
		{
			if(FillOpenFaceArgumentsForWideString(inSecondaryFilePath,attachStreamArguments) != eSuccess)
			{
				DoneFace(face);
				face = NULL;
				break;
			}

			FT_Error ftStatus = FT_Attach_Stream(face,&attachStreamArguments);
			if(ftStatus != 0)
			{
				TRACE_LOG1("FreeTypeWrapper::NewFace, unable to load secondary file %s",inSecondaryFilePath.c_str());
				TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ft_errors[ftStatus].err_code,StringTraits(ft_errors[ftStatus].err_msg).WidenString().c_str());
				DoneFace(face);
				face = NULL;
			}
		}while(false);

		if(!face)
			CloseOpenFaceArgumentsStream(attachStreamArguments);
	}

	return face;	
}


FT_Error FreeTypeWrapper::DoneFace(FT_Face ioFace)
{
	return FT_Done_Face(ioFace);
}

FT_Library FreeTypeWrapper::operator->()
{
	return mFreeType;
}

static unsigned long InputFileReadSeek(	   FT_Stream	   stream,
										   unsigned long   offset,
										   unsigned char*  buffer,
										   unsigned long   count)
{
	IByteReader* inputFileStream = ((InputFile*)(stream->descriptor.pointer))->GetInputStream();	
	unsigned long readBytes = 0;
	
	inputFileStream->SetPosition(offset);
	if(count > 0)
		readBytes = (unsigned long)inputFileStream->Read(buffer,count);
	return readBytes;
}

static void InputFileClose(FT_Stream  stream)
{
	delete (InputFile*)(stream->descriptor.pointer);
	stream->descriptor.pointer = NULL;
}

FT_Stream FreeTypeWrapper::CreateFTStreamForPath(const wstring& inFilePath)
{
	InputFile* inputFile = new InputFile;

	if(inputFile->OpenFile(inFilePath) != eSuccess)
		return NULL;

	FT_Stream aStream = new FT_StreamRec();

	aStream->base = NULL;
	aStream->size = (unsigned long)inputFile->GetFileSize();
	aStream->pos = 0;
	aStream->descriptor.pointer = inputFile;
	aStream->pathname.pointer = NULL;
	aStream->read = InputFileReadSeek;
	aStream->close = InputFileClose;
	aStream->memory = NULL;
	aStream->cursor = NULL;
	aStream->limit = NULL;

	return aStream;
}

FreeTypeWrapper::operator FT_Library() const
{
	return mFreeType;
}

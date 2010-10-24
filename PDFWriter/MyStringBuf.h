#pragma once

#include <sstream>

using namespace std;

class MyStringBuf : public stringbuf
{
public:

	stringbuf::pos_type GetCurrentReadPosition() {return gptr()-eback();}
	stringbuf::pos_type GetCurrentWritePosition() {return pptr()-pbase();}
	
	stringbuf::pos_type GetReadSize() {return egptr()-eback();}
	stringbuf::pos_type GetWriteSize() {return epptr()-pbase();}

};
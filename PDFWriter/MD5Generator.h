/*
 Additioncal Copyright Information:

 Gal Kahana 8/5/2010. my code is completely copied/adapted from this:

 MD5.CC - source code for the C++/object oriented translation and 
          modification of MD5.

 Translation and modification (c) 1995 by Mordechai T. Abzug 

 This translation/ modification is provided "as is," without express or 
 implied warranty of any kind.

 The translator/ modifier does not claim (1) that MD5 will do what you think 
 it does; (2) that this translation/ modification is accurate; or (3) that 
 this software is "merchantible."  (Language for this disclaimer partially 
 copied from the disclaimer below).

 the code is based on:

 MD5.H - header file for MD5C.C
 MDDRIVER.C - test driver for MD2, MD4 and MD5

 Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 rights reserved.

 License to copy and use this software is granted provided that it
 is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 Algorithm" in all material mentioning or referencing this software
 or this function.

 License is also granted to make and use derivative works provided
 that such works are identified as "derived from the RSA Data
 Security, Inc. MD5 Message-Digest Algorithm" in all material
 mentioning or referencing the derived work.

 RSA Data Security, Inc. makes no representations concerning either
 the merchantability of this software or the suitability of this
 software for any particular purpose. It is provided "as is"
 without express or implied warranty of any kind.

 These notices must be retained in any copies of any part of this
 documentation and/or software.
*/
#pragma once

#include "EStatusCode.h"
#include <string>

using namespace std;

class MD5Generator
{
public:
	MD5Generator(void);
	~MD5Generator(void);

	EStatusCode Accumulate(const string& inString);
	EStatusCode Accumulate(const wstring& inString);

	string ToString();

private:

	typedef unsigned       int uint4; 
	typedef unsigned short int uint2; 
	typedef unsigned      char uint1; 
	string MD5FinalString;

	uint4 mState[4];
	uint4 mCount[2];     // number of *bits*, mod 2^64
	uint1 mBuffer[64];   // input buffer
	uint1 mDigest[16];
	bool mIsFinalized;

	void Accumulate(const uint1* inBlock,unsigned long inBlockSize);

	void Transform(const uint1 *inBuffer);  // does the real update work.  Note 
                                   // that length is implied to be 64.

	void Decode (const uint1 *inInput, uint4 inInputLen,uint4 *outOutput);

	unsigned int RotateLeft(uint4 x, uint4 n);
	unsigned int F(uint4 x, uint4 y, uint4 z);
	unsigned int G(uint4 x, uint4 y, uint4 z);
	unsigned int H(uint4 x, uint4 y, uint4 z);
	unsigned int I(uint4 x, uint4 y, uint4 z);
	void FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4  s, uint4 ac);
	void GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	void HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	void II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	void Encode(uint1 *output, uint4 *input, uint4 len);
	void SetFinalStringToHex();

	static const uint1 PADDING[64];

};

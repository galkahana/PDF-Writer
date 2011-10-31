/*
   Source File : PDFLiteralString.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
#pragma once

#include "PDFObject.h"

#include <string>

using namespace std;

class PDFLiteralString : public PDFObject
{
public:

	enum EType
	{
		eType = ePDFObjectLiteralString
	};

	// Value is the interpreted string (no enclosing paranthesis and escaped sequances as the result values)
	PDFLiteralString(const string& inValue);
	virtual ~PDFLiteralString(void);

	const string& GetValue() const;
	operator string() const;

private:
	string mValue;

};

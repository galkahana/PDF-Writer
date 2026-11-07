/*
   Source File : DecryptionHelperTest.cpp


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
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "OutputStringBufferStream.h"
#include "InputByteArrayStream.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFStreamInput.h"
#include "PDFLiteralString.h"
#include "PDFTextString.h"
#include "DecryptionHelper.h"
#include "IByteReader.h"
#include "RefCountPtr.h"
#include "EStatusCode.h"

#include <iostream>
#include <string>

using namespace std;
using namespace PDFHummus;

// Distinct user and owner passwords, so that authenticating with the owner
// password goes through recovering the user password from /O rather than
// matching the user password directly.
static const char* scUserPassword = "user";
static const char* scOwnerPassword = "owner";
static const char* scWrongPassword = "wrong";

static const char* scTitle = "Decryption Round Trip";
static const char* scPageContent = "0 0 m 100 100 l S\n";

static const long long scPrintFlag = 4;
static const long long scPrintAndR3OnlyFlags = 0xF04;

// ---- helpers --------------------------------------------------------------

// Writes a one page PDF, encrypted with scUserPassword/scOwnerPassword, into
// outPDF. The page content stream and the info dictionary /Title are the
// known plaintexts read back by the tests. 
static EStatusCode WriteEncryptedPDF(EPDFVersion inVersion, long long inProtectionFlags, string& outPDF)
{
	OutputStringBufferStream outputStream;
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDFForStream(
			&outputStream,
			inVersion,
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(false, true, EncryptionOptions(scUserPassword, inProtectionFlags, scOwnerPassword)));
		if(status != eSuccess)
			break;

		pdfWriter.GetDocumentContext().GetTrailerInformation().GetInfo().Title = PDFTextString(scTitle);

		PDFPage page;
		page.SetMediaBox(PDFRectangle(0, 0, 595, 842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(&page);
		if(contentContext == NULL)
		{
			status = eFailure;
			break;
		}
		contentContext->WriteFreeCode(scPageContent);

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePage(&page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDFForStream();
		if(status != eSuccess)
			break;

		outPDF = outputStream.ToString();
	} while(false);

	return status;
}

static string ReadInfoTitle(PDFParser& inParser)
{
	if(inParser.GetTrailer() == NULL)
		return "";

	RefCountPtr<PDFDictionary> info(
		(PDFDictionary*)inParser.QueryDictionaryObject(inParser.GetTrailer(), "Info"));
	if(!info || info->GetType() != PDFObject::ePDFObjectDictionary)
		return "";

	RefCountPtr<PDFObject> title(inParser.QueryDictionaryObject(info.GetPtr(), "Title"));
	if(!title || title->GetType() != PDFObject::ePDFObjectLiteralString)
		return "";

	return PDFTextString(((PDFLiteralString*)title.GetPtr())->GetValue()).ToUTF8String();
}

static string ReadFirstPageContent(PDFParser& inParser)
{
	if(inParser.GetPagesCount() == 0)
		return "";

	RefCountPtr<PDFDictionary> pageDict(inParser.ParsePage(0));
	if(!pageDict)
		return "";

	RefCountPtr<PDFObject> contents(inParser.QueryDictionaryObject(pageDict.GetPtr(), "Contents"));
	if(!contents || contents->GetType() != PDFObject::ePDFObjectStream)
		return "";

	IByteReader* reader = inParser.StartReadingFromStream((PDFStreamInput*)contents.GetPtr());
	if(reader == NULL)
		return "";

	string result;
	IOBasicTypes::Byte buffer[256];
	while(reader->NotEnded())
	{
		IOBasicTypes::LongBufferSizeType readAmount = reader->Read(buffer, sizeof(buffer));
		result.append((const char*)buffer, (size_t)readAmount);
	}
	delete reader;

	return result;
}

// ---- Setup ----------------------------------------------------------------

struct ValidPasswordCase
{
	const char* mLabel;
	EPDFVersion mVersion;
	long long mProtectionFlags;
	const char* mPassword;
	bool mExpectOwner;
};

// ePDFVersion13 -> V1 (RC4 40 bit), R2 or R3 depending on flags
// ePDFVersion14 -> V2 (RC4 128 bit), R3
// ePDFVersion16 -> V4 (AESV2), R4
// ePDFVersion20 -> V5 (AESV3), R6
static const ValidPasswordCase scSetupValidPasswordCases[] = {
	{"R2UserPassword_DecryptsContent",  ePDFVersion13, scPrintFlag, scUserPassword, false},
	{"R2OwnerPassword_DecryptsContent", ePDFVersion13, scPrintFlag, scOwnerPassword, true},
	{"V1R3UserPassword_DecryptsContent",  ePDFVersion13, scPrintAndR3OnlyFlags, scUserPassword, false},
	{"V1R3OwnerPassword_DecryptsContent", ePDFVersion13, scPrintAndR3OnlyFlags, scOwnerPassword, true},
	{"V2R3UserPassword_DecryptsContent",  ePDFVersion14, scPrintFlag, scUserPassword, false},
	{"V2R3OwnerPassword_DecryptsContent", ePDFVersion14, scPrintFlag, scOwnerPassword, true},
	{"R4AESV2UserPassword_DecryptsContent",  ePDFVersion16, scPrintFlag, scUserPassword, false},
	{"R4AESV2OwnerPassword_DecryptsContent", ePDFVersion16, scPrintFlag, scOwnerPassword, true},
#ifndef PDFHUMMUS_NO_OPENSSL
	{"R6UserPassword_DecryptsContent",  ePDFVersion20, scPrintFlag, scUserPassword, false},
	{"R6OwnerPassword_DecryptsContent", ePDFVersion20, scPrintFlag, scOwnerPassword, true},
#endif
};

struct WrongPasswordCase
{
	const char* mLabel;
	EPDFVersion mVersion;
	long long mProtectionFlags;
};

static const WrongPasswordCase scSetupWrongPasswordCases[] = {
	{"R2WrongPassword_FailsVerification", ePDFVersion13, scPrintFlag},
	{"V1R3WrongPassword_FailsVerification", ePDFVersion13, scPrintAndR3OnlyFlags},
	{"V2R3WrongPassword_FailsVerification", ePDFVersion14, scPrintFlag},
	{"R4AESV2WrongPassword_FailsVerification", ePDFVersion16, scPrintFlag},
#ifndef PDFHUMMUS_NO_OPENSSL
	{"R6WrongPassword_FailsVerification", ePDFVersion20, scPrintFlag},
#endif
};

static bool ExpectAllDecryptKnownContent(const char* inFunction, const ValidPasswordCase* inCases, size_t inCount)
{
	for(size_t i = 0; i < inCount; ++i)
	{
		const ValidPasswordCase& c = inCases[i];

		// Arrange
		string pdf;
		if(WriteEncryptedPDF(c.mVersion, c.mProtectionFlags, pdf) != eSuccess)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: failed to write encrypted PDF\n";
			return false;
		}
		InputByteArrayStream inputStream((const IOBasicTypes::Byte*)pdf.data(), (LongFilePositionType)pdf.size());
		PDFParser parser;

		// Act
		EStatusCode status = parser.StartPDFParsing(&inputStream, PDFParsingOptions(c.mPassword));

		// Assert
		if(status != eSuccess)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: failed to start parsing\n";
			return false;
		}

		// Act
		string title = ReadInfoTitle(parser);
		string content = ReadFirstPageContent(parser);

		// Assert
		if(!parser.IsEncryptionSupported())
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: password did not authenticate\n";
			return false;
		}
		if(parser.GetDecryptionHelper().DidSucceedOwnerPasswordVerification() != c.mExpectOwner)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: expected owner verification to be " << c.mExpectOwner << "\n";
			return false;
		}
		if(title != scTitle)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: expected title \"" << scTitle << "\", got \"" << title << "\"\n";
			return false;
		}
		if(content != scPageContent)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: expected page content \"" << scPageContent << "\", got \"" << content << "\"\n";
			return false;
		}
	}
	return true;
}

static bool ExpectAllFailVerification(const char* inFunction, const WrongPasswordCase* inCases, size_t inCount)
{
	for(size_t i = 0; i < inCount; ++i)
	{
		const WrongPasswordCase& c = inCases[i];

		// Arrange
		string pdf;
		if(WriteEncryptedPDF(c.mVersion, c.mProtectionFlags, pdf) != eSuccess)
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: failed to write encrypted PDF\n";
			return false;
		}
		InputByteArrayStream inputStream((const IOBasicTypes::Byte*)pdf.data(), (LongFilePositionType)pdf.size());
		PDFParser parser;

		// Act
		parser.StartPDFParsing(&inputStream, PDFParsingOptions(scWrongPassword));

		// Assert
		if(!parser.GetDecryptionHelper().DidFailPasswordVerification())
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: expected password verification to fail\n";
			return false;
		}
		if(parser.IsEncryptionSupported())
		{
			cout << "[" << inFunction << "::" << c.mLabel << "]: expected document to be undecryptable\n";
			return false;
		}
	}
	return true;
}

static bool RunSetupCases()
{
	return ExpectAllDecryptKnownContent(
				"Setup",
				scSetupValidPasswordCases,
				sizeof(scSetupValidPasswordCases) / sizeof(scSetupValidPasswordCases[0])) &&
			ExpectAllFailVerification(
				"Setup",
				scSetupWrongPasswordCases,
				sizeof(scSetupWrongPasswordCases) / sizeof(scSetupWrongPasswordCases[0]));
}

int DecryptionHelperTest(int argc, char* argv[])
{
	if(!RunSetupCases())
		return 1;

	return 0;
}

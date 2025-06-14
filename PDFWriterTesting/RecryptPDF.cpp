/*
   Source File : TIFFImageTest.cpp


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

#include <iostream>

#include "testing/TestIO.h"

int RecryptPDF(int argc, char* argv[])
{
	EStatusCode status; 

	do
	{
		// recrypt a document with AES encryption, and remove it
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"china.pdf"),
			"",
			BuildRelativeOutputPath(argv,"chinaWithoutEncryption.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF\n";
			break;
		}

		// recrypt an encrypted document with no password
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"PDFWithPassword.pdf"),
			"user",
			BuildRelativeOutputPath(argv,"RecryptPDFWithPasswordToNothing.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true,true));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF\n";
			break;
		}

#ifndef PDFHUMMUS_NO_OPENSSL
		// same but for a PDF2.0 fellow
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"PDFWithPassword20.pdf"),
			"user",
			BuildRelativeOutputPath(argv,"RecryptPDFWithPasswordToNothingPDF20.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true),
			ePDFVersion20);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF2.0\n";
			break;
		}		
		
		// again with the owner password
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"PDFWithPassword20.pdf"),
			"owner",
			BuildRelativeOutputPath(argv,"RecryptPDFWithPasswordToNothingOwnerPDF20.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true),
			ePDFVersion20);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF2.0 with owner password\n";
			break;
		}				
#endif

		// recrypt an encrypted document with new password
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"PDFWithPassword.pdf"),
			"user",
			BuildRelativeOutputPath(argv,"RecryptPDFWithPasswordToNewPassword.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true,true,EncryptionOptions("user1",4,"owner1")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

#ifndef PDFHUMMUS_NO_OPENSSL
		// again, but for a PDF2.0 fellow
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"PDFWithPassword20.pdf"),
			"user",
			BuildRelativeOutputPath(argv,"RecryptPDFWithPasswordToNewPasswordPDF20.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1"))
		);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF2.0 with new password PDF\n";
			break;
		}
#endif

		// recrypt a plain to document to one with password
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"Original.pdf"),
			"",
			BuildRelativeOutputPath(argv,"RecryptPDFOriginalToPasswordProtected.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

		// same, but forcing AES
		status = PDFWriter::RecryptPDF(
			BuildRelativeInputPath(argv,"Original.pdf"),
			"",
			BuildRelativeOutputPath(argv,"RecryptPDFOriginalToPasswordProtectedAES.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")),
			ePDFVersion16);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

	}while(false);

	return status == eSuccess ? 0:1;	
}

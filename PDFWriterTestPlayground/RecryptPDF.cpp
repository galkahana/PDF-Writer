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

#include "RecryptPDF.h"
#include "TestsRunner.h"
#include "PDFWriter.h"

#include <iostream>


RecryptPDF::RecryptPDF(void)
{
}

RecryptPDF::~RecryptPDF(void)
{
}

EStatusCode RecryptPDF::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status; 

	do
	{
		// recrypt a document with AES encryption, and remove it
		status = PDFWriter::RecryptPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/china.pdf"),
			"",
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "chinaWithoutEncryption.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF\n";
			break;
		}

		// recrypt an encrypted document with no password
		status = PDFWriter::RecryptPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/PDFWithPassword.pdf"),
			"user",
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "RecryptPDFWithPasswordToNothing.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true,true));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to decrypt PDF\n";
			break;
		}

		// recrypt an encrypted document with new password
		status = PDFWriter::RecryptPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/PDFWithPassword.pdf"),
			"user",
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "RecryptPDFWithPasswordToNewPassword.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true,true,EncryptionOptions("user1",4,"owner1")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

		// recrypt a plain to document to one with password
		status = PDFWriter::RecryptPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/Original.pdf"),
			"",
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "RecryptPDFOriginalToPasswordProtected.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

		// same, but forcing AES
		status = PDFWriter::RecryptPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/Original.pdf"),
			"",
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "RecryptPDFOriginalToPasswordProtectedAES.pdf"),
			LogConfiguration::DefaultLogConfiguration(),
			PDFCreationSettings(true, true, EncryptionOptions("user1", 4, "owner1")),
			ePDFVersion16);
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to encrypt PDF with new password PDF\n";
			break;
		}

	}while(false);

	return status;	
}

ADD_CATEGORIZED_TEST(RecryptPDF,"Xcryption")

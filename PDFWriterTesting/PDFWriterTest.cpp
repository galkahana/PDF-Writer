/*
   Source File : PDFWriterTest.cpp


   Copyright 2026 Gal Kahana PDFWriter

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

#include "testing/TestIO.h"

#include <iostream>

#ifndef PDFHUMMUS_NO_OPENSSL
#include "RandomGenerator.h"
#endif

using namespace std;
using namespace PDFHummus;

#ifndef PDFHUMMUS_NO_OPENSSL

static int AlwaysFailingRandBytes(unsigned char* outBuffer, int inSize)
{
    (void)outBuffer;
    (void)inSize;
    return 0;
}

static bool StartPDF_PDF20EncryptionWithWorkingCSPRNG_Succeeds(char* argv[])
{
    // Arrange
    string outputPath = BuildRelativeOutputPath(argv, "StartPDFCSPRNGSuccess.pdf");
    PDFWriter pdfWriter;
    PDFCreationSettings creationSettings(true, true, EncryptionOptions("user", 0, "owner"), false);

    // Act
    EStatusCode status = pdfWriter.StartPDF(outputPath, ePDFVersion20, LogConfiguration::DefaultLogConfiguration(), creationSettings);
    if (status == eSuccess)
        status = pdfWriter.EndPDF();

    // Assert
    if (status != eSuccess)
    {
        cout << "PDFWriterTest [StartPDF_PDF20EncryptionWithWorkingCSPRNG_Succeeds]: StartPDF/EndPDF failed with a working CSPRNG" << endl;
        return false;
    }
    return true;
}

static bool StartPDF_PDF20EncryptionWithFailingCSPRNG_ReturnsFailure(char* argv[])
{
    // Arrange
    string outputPath = BuildRelativeOutputPath(argv, "StartPDFCSPRNGFailure.pdf");
    RandomGenerator::SetRandBytesFunc(AlwaysFailingRandBytes);
    PDFWriter pdfWriter;
    PDFCreationSettings creationSettings(true, true, EncryptionOptions("user", 0, "owner"), false);

    // Act
    EStatusCode status = pdfWriter.StartPDF(outputPath, ePDFVersion20, LogConfiguration::DefaultLogConfiguration(), creationSettings);
    RandomGenerator::SetRandBytesFunc(NULL); // restore real RAND_bytes for any other test in this process

    // Assert
    if (status == eSuccess)
    {
        cout << "PDFWriterTest [StartPDF_PDF20EncryptionWithFailingCSPRNG_ReturnsFailure]: StartPDF succeeded despite a forced CSPRNG failure" << endl;
        return false;
    }
    return true;
}

#endif // PDFHUMMUS_NO_OPENSSL

int PDFWriterTest(int argc, char* argv[])
{
    (void)argc;
#ifndef PDFHUMMUS_NO_OPENSSL
    if (!StartPDF_PDF20EncryptionWithWorkingCSPRNG_Succeeds(argv)) return 1;
    if (!StartPDF_PDF20EncryptionWithFailingCSPRNG_ReturnsFailure(argv)) return 1;
#else
    (void)argv;
#endif
    return 0;
}

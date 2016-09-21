#include "EncryptedPDF.h"
#include "TestsRunner.h"
#include "PDFParser.h"
#include "InputFile.h"
#include "PDFDocumentCopyingContext.h"
#include "PDFWriter.h"
#include <iostream>

using namespace std;
using namespace PDFHummus;

EncryptedPDFTester::EncryptedPDFTester(void)
{
}

EncryptedPDFTester::~EncryptedPDFTester(void)
{
}

EStatusCode EncryptedPDFTester::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = PDFHummus::eSuccess;
	InputFile pdfFile;
	PDFWriter pdfWriter;
	PDFDocumentCopyingContext* copyingContext = NULL;


	PDFParser parser;

	do
	{
		status = pdfWriter.StartPDF(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PDFWithPasswordDecrypted.pdf"), ePDFVersion13,
			LogConfiguration(true, true,
				RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PDFWithPasswordDecrypted.txt")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		copyingContext = pdfWriter.CreatePDFCopyingContext(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/PDFWithPassword.pdf"),
			PDFParsingOptions("user"));
		if (!copyingContext)
		{
			cout << "failed to initialize copying context from PDFWithPassword.pdf\n";
			status = PDFHummus::eFailure;
			break;
		}

		unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
		for (unsigned long i = 0; i < pagesCount && status == PDFHummus::eSuccess; ++i) {
			EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(i);
			if (result.first != PDFHummus::eSuccess)
			{
				cout << "failed to append page "<<(i+1)<<" from PDFWithPassword.pdf\n";
				status = result.first;
				break;
			}

		}

		delete copyingContext;
		copyingContext = NULL;

		status = pdfWriter.EndPDF();
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed in end PDF\n";
			break;
		}

	} while (false);

	delete copyingContext;
	return status;
}

ADD_CATEGORIZED_TEST(EncryptedPDFTester, "Xcryption")
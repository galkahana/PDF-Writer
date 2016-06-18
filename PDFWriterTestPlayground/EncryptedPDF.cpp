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
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "031680580_2016_05Decrypted.PDF"), ePDFVersion13,
			LogConfiguration(true, true,
				RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "031680580_2016_05Decrypted.txt")));
		if (status != PDFHummus::eSuccess)
		{
			cout << "failed to start PDF\n";
			break;
		}

		copyingContext = pdfWriter.CreatePDFCopyingContext(
			RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "TestMaterials/031680580_2016_05.PDF"),
			PDFParsingOptions("G5MJ14"));
		if (!copyingContext)
		{
			cout << "failed to initialize copying context from 031680580_2016_05.pdf\n";
			status = PDFHummus::eFailure;
			break;
		}

		unsigned long pagesCount = copyingContext->GetSourceDocumentParser()->GetPagesCount();
		for (unsigned long i = 0; i < pagesCount && status == PDFHummus::eSuccess; ++i) {
			EStatusCodeAndObjectIDType result = copyingContext->AppendPDFPageFromPDF(i);
			if (result.first != PDFHummus::eSuccess)
			{
				cout << "failed to append page "<<(i+1)<<" from 031680580_2016_05.pdf\n";
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

ADD_CATEGORIZED_TEST(EncryptedPDFTester, "PDFEmbedding")
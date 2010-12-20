#include "PFBStreamTest.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "InputPFBDecodeStream.h"
#include "OutputStreamTraits.h"
#include "IByteWriterWithPosition.h"
#include "TestsRunner.h"

#include <iostream>

using namespace std;

PFBStreamTest::PFBStreamTest(void)
{
}

PFBStreamTest::~PFBStreamTest(void)
{
}

EStatusCode PFBStreamTest::Run()
{
	EStatusCode status;
	InputFile pfbFile;
	OutputFile decodedPFBFile;
	InputPFBDecodeStream decodeStream;

	do
	{
		pfbFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\fonts\\HLB_____.PFB");

		decodedPFBFile.OpenFile(L"C:\\PDFLibTests\\decodedPFBFile.txt");


		status = decodeStream.Assign(pfbFile.GetInputStream());
		
		if(status != eSuccess)
		{
			wcout<<"Failed to assign pfb input stream";
			break;
		}

		OutputStreamTraits traits(decodedPFBFile.GetOutputStream());

		status = traits.CopyToOutputStream(&decodeStream);

		if(status != eSuccess)
		{
			wcout<<"Failed to decode pfb stream";
			break;
		}
	}while(false);

	decodeStream.Assign(NULL);
	pfbFile.CloseFile();
	decodedPFBFile.CloseFile();

	return status;
}


ADD_CETEGORIZED_TEST(PFBStreamTest,"Type1")
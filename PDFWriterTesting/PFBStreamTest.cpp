/*
   Source File : PFBStreamTest.cpp


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
#include "InputFile.h"
#include "OutputFile.h"
#include "InputPFBDecodeStream.h"
#include "OutputStreamTraits.h"
#include "IByteWriterWithPosition.h"

#include <iostream>

#include "testing/TestIO.h"

using namespace std;
using namespace PDFHummus;


int PFBStreamTest(int argc, char* argv[])
{
	EStatusCode status;
	InputFile pfbFile;
	OutputFile decodedPFBFile;
	InputPFBDecodeStream decodeStream;

	do
	{
		pfbFile.OpenFile(BuildRelativeInputPath(argv,"fonts/HLB_____.PFB"));

		decodedPFBFile.OpenFile(BuildRelativeOutputPath(argv,"decodedPFBFile.txt"));


		status = decodeStream.Assign(pfbFile.GetInputStream());
		
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to assign pfb input stream";
			break;
		}

		OutputStreamTraits traits(decodedPFBFile.GetOutputStream());

		status = traits.CopyToOutputStream(&decodeStream);

		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to decode pfb stream";
			break;
		}
	}while(false);

	decodeStream.Assign(NULL);
	pfbFile.CloseFile();
	decodedPFBFile.CloseFile();

	return status == eSuccess ? 0:1;
}

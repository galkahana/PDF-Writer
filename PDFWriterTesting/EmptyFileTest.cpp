/*
   Source File : EmptyFileTest.cpp


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

using namespace std;
using namespace PDFHummus;



int EmptyFileTest(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	LogConfiguration logConfiguration(true,true,BuildRelativeOutputPath(argv,"EmptyFileLog.txt"));
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(BuildRelativeOutputPath(argv,"test.txt"),ePDFVersion13,logConfiguration);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	
		
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status == eSuccess ? 0:1;
}

/*
   Source File : TestMeasurementsTest.cpp


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
#include "TestMeasurementsTest.h"
#include "PDFWriter.h"
#include "TestsRunner.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFUsedFont.h"

#include <iostream>

using namespace PDFHummus;

TestMeasurementsTest::TestMeasurementsTest(void)
{
}


TestMeasurementsTest::~TestMeasurementsTest(void)
{
}

EStatusCode TestMeasurementsTest::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TextMeasurementsTest.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TextMeasurementsTest.log")));
		if(status != eSuccess)
		{
			cout<<"Failed to start file\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
		
		PageContentContext* cxt = pdfWriter.StartPageContentContext(page);
		PDFUsedFont* arialFont = pdfWriter.GetFontForFile(
                                                     RelativeURLToLocalPath(
                                                                            inTestConfiguration.mSampleFileBase,
                                                                            "TestMaterials/fonts/arial.ttf"));
		if(!arialFont)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font for arial font\n";
			break;
		}

		AbstractContentContext::GraphicOptions pathStrokeOptions(AbstractContentContext::eStroke,
																AbstractContentContext::eRGB,
																AbstractContentContext::ColorValueForName("DarkMagenta"),
																4);
		AbstractContentContext::TextOptions textOptions(arialFont,
														14,
														AbstractContentContext::eGray,
														0);

		PDFUsedFont::TextMeasures textDimensions = arialFont->CalculateTextDimensions("Hello World",14);

		cxt->WriteText(10,100,"Hello World",textOptions);
		DoubleAndDoublePairList pathPoints;
		pathPoints.push_back(DoubleAndDoublePair(10+textDimensions.xMin,98+textDimensions.yMin));
		pathPoints.push_back(DoubleAndDoublePair(10+textDimensions.xMax,98+textDimensions.yMin));
		cxt->DrawPath(pathPoints,pathStrokeOptions);
		pathPoints.clear();
		pathPoints.push_back(DoubleAndDoublePair(10+textDimensions.xMin,102+textDimensions.yMax));
		pathPoints.push_back(DoubleAndDoublePair(10+textDimensions.xMax,102+textDimensions.yMax));
		cxt->DrawPath(pathPoints,pathStrokeOptions);

		status = pdfWriter.EndPageContentContext(cxt);
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to end content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to write page\n";
			break;
		}


		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to end pdf\n";
			break;
		}

	}while(false);


	return status;
}

ADD_CATEGORIZED_TEST(TestMeasurementsTest,"Text")

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
#include "HighLevelContentContext.h"
#include "PDFWriter.h"
#include "TestsRunner.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFUsedFont.h"

#include <iostream>

using namespace PDFHummus;


HighLevelContentContext::HighLevelContentContext(void)
{
}


HighLevelContentContext::~HighLevelContentContext(void)
{
}

EStatusCode HighLevelContentContext::Run(const TestConfiguration& inTestConfiguration)
{
	EStatusCode status = eSuccess;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"HighLevelContentContext.pdf"),
                                    ePDFVersion13,
                                    LogConfiguration(true,true,
                                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"HighLevelContentContext.log")));
		if(status != eSuccess)
		{
			cout<<"Failed to start file\n";
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
		
		PageContentContext* cxt = pdfWriter.StartPageContentContext(page);

		AbstractContentContext::TextOptions textOptions(pdfWriter.GetFontForFile(
																			RelativeURLToLocalPath(
                                                                            inTestConfiguration.mSampleFileBase,
                                                                            "TestMaterials/fonts/arial.ttf")),
																			14,
																			AbstractContentContext::eGray,
																			0);
		AbstractContentContext::GraphicOptions pathFillOptions(AbstractContentContext::eFill,
																AbstractContentContext::eCMYK,
																0xFF000000);
		AbstractContentContext::GraphicOptions pathStrokeOptions(AbstractContentContext::eStroke,
																AbstractContentContext::eRGB,
																AbstractContentContext::ColorValueForName("DarkMagenta"),
																4);

		DoubleAndDoublePairList pathPoints;

		// draw path
		pathPoints.push_back(DoubleAndDoublePair(75,640));
		pathPoints.push_back(DoubleAndDoublePair(149,800));
		pathPoints.push_back(DoubleAndDoublePair(225,640));
		cxt->DrawPath(pathPoints,pathFillOptions);
		pathPoints.clear();
		pathPoints.push_back(DoubleAndDoublePair(75,540));
		pathPoints.push_back(DoubleAndDoublePair(110,440));
		pathPoints.push_back(DoubleAndDoublePair(149,540));
		pathPoints.push_back(DoubleAndDoublePair(188,440));
		pathPoints.push_back(DoubleAndDoublePair(223,540));
		cxt->DrawPath(pathPoints,pathStrokeOptions);

		// draw square
		cxt->DrawSquare(375,640,120,pathFillOptions);
		cxt->DrawSquare(375,440,120,pathStrokeOptions);

		// draw rectangle
		cxt->DrawRectangle(375,220,50,160,pathFillOptions);
		cxt->DrawRectangle(375,10,50,160,pathStrokeOptions);

		// draw circle
		cxt->DrawCircle(149,300,80,pathFillOptions);
		cxt->DrawCircle(149,90,80,pathStrokeOptions);

		// wrote text (writing labels for each of the shapes)
		cxt->WriteText(75,805,"Paths",textOptions);
		cxt->WriteText(375,805,"Squares",textOptions);
		cxt->WriteText(375,400,"Rectangles",textOptions);
		cxt->WriteText(75,400,"Circles",textOptions);

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


ADD_CATEGORIZED_TEST(HighLevelContentContext,"PDF")

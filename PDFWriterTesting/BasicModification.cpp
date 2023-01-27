/*
 Source File : BasicModification.cpp
 
 
 Copyright 2012 Gal Kahana PDFWriter
 
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
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "OutputStreamTraits.h"
#include "InputFile.h"
#include "OutputFile.h"

#include "testing/TestIO.h"

#include <iostream>

using namespace PDFHummus;



static EStatusCode TestBasicFileModification(char* argv[],string inSourceFileName)
{
	PDFWriter pdfWriter;
	EStatusCode status = eSuccess; 
	do
	{
 		status = pdfWriter.ModifyPDF(
                                     BuildRelativeInputPath(argv,inSourceFileName + string(".pdf")),
                                     ePDFVersion13,
                                     BuildRelativeOutputPath(argv,string("Modified") + inSourceFileName + string(".pdf")),
                                     LogConfiguration(true,true,BuildRelativeOutputPath(argv,string("Modified") + inSourceFileName + string(".log"))));  
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	        
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
			break;
		}
        
		PDFUsedFont* font = pdfWriter.GetFontForFile(
                                                     BuildRelativeInputPath(argv,"fonts/couri.ttf"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for couri.ttf\n";
			break;
		}
        
        
		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);
        
		contentContext->Tf(font,1);
        
		contentContext->Tm(30,0,0,30,78.4252,662.8997);
        
		EStatusCode encodingStatus = contentContext->Tj("about");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font";
        
		// continue even if failed...want to see how it looks like
		contentContext->ET();
        
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page\n";
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
    
	return status;	
    
    
}

static EStatusCode TestInPlaceFileModification(char* argv[],string inSourceFileName)
{
	PDFWriter pdfWriter;
	EStatusCode status = eSuccess; 
	do
	{
        // first copy source file to target
        {
            InputFile sourceFile;
            
            status = sourceFile.OpenFile(
                                         BuildRelativeInputPath(argv,inSourceFileName + string(".pdf")));
            if(status != eSuccess)
            {
                cout<<"failed to open source PDF\n";
                break;                                         
            }
                 
            OutputFile targetFile;
            
            status = targetFile.OpenFile(BuildRelativeOutputPath(argv,string("InPlaceModified") + inSourceFileName + string(".pdf")));
            if(status != eSuccess)
            {
                cout<<"failed to open target PDF\n";
                break;                                         
            }        
            
            OutputStreamTraits traits(targetFile.GetOutputStream());
            status = traits.CopyToOutputStream(sourceFile.GetInputStream());
            if(status != eSuccess)
            {
                cout<<"failed to copy source to target PDF\n";
                break;                                         
            }         
            
            sourceFile.CloseFile();
            targetFile.CloseFile();
            
        }
        
        // now modify in place
        
        
 		status = pdfWriter.ModifyPDF(
                                     BuildRelativeOutputPath(argv,string("InPlaceModified") + inSourceFileName + string(".pdf")),
                                     ePDFVersion13,
                                     "",
                                     LogConfiguration(true,true,BuildRelativeOutputPath(argv,string("InPlaceModified") + inSourceFileName + string(".log"))));  
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	        
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == contentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
			break;
		}
        
		PDFUsedFont* font = pdfWriter.GetFontForFile(
                                                     BuildRelativeInputPath(argv,"fonts/couri.ttf"));
		if(!font)
		{
			status = PDFHummus::eFailure;
			cout<<"Failed to create font object for couri.ttf\n";
			break;
		}
        
        
		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);
        
		contentContext->Tf(font,1);
        
		contentContext->Tm(30,0,0,30,78.4252,662.8997);
        
		EStatusCode encodingStatus = contentContext->Tj("about");
		if(encodingStatus != PDFHummus::eSuccess)
			cout<<"Could not find some of the glyphs for this font";
        
		// continue even if failed...want to see how it looks like
		contentContext->ET();
        
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to end page content context\n";
			break;
		}
        
		status = pdfWriter.WritePageAndRelease(page);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to write page\n";
			break;
		}
        
		status = pdfWriter.EndPDF();
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
	}while(false);
    
	return status;	
}



int BasicModification(int argc, char* argv[])
{
    EStatusCode status = eSuccess;
    
    do 
    {
        // modification of different source and target
        
        
        if(TestBasicFileModification(argv, "BasicTIFFImagesTest") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for BasicTIFFImagesTest\n";            
        }

        if(TestBasicFileModification(argv, "Linearized") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for Linearized\n";            
        }   

        if(TestBasicFileModification(argv, "MultipleChange") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for MultipleChange\n";            
        }     
 
        if(TestBasicFileModification(argv, "RemovedItem") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for RemovedItem\n";            
        }    
        
        if(TestBasicFileModification(argv, "Protected") == eSuccess)
        {
            status = eFailure;
			cout<<"should have failed on modification for Protected, but didn't\n";                
        }
        else 
        {
            cout<<"succeeded in failing for protected file modification\n";
        }
        
        
        if(TestBasicFileModification(argv, "ObjectStreams") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for ObjectStreams\n";            
        }     

        if(TestBasicFileModification(argv, "ObjectStreamsModified") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for ObjectStreamsModified\n";            
        }       
        
        // in place modification
        
        
        if(TestInPlaceFileModification(argv, "BasicTIFFImagesTest") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for BasicTIFFImagesTest\n";            
        }
        
        if(TestInPlaceFileModification(argv, "Linearized") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for Linearized\n";            
        }   
        
        if(TestInPlaceFileModification(argv, "MultipleChange") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for MultipleChange\n";            
        }     
        
        if(TestInPlaceFileModification(argv, "RemovedItem") != eSuccess)
        {
            status = eFailure;
			cout<<"failed modification for RemovedItem\n";            
        }    
        
        if(TestInPlaceFileModification(argv, "ObjectStreams") != eSuccess)
         {
         status = eFailure;
         cout<<"failed modification for ObjectStreams\n";            
         }     
         
         if(TestInPlaceFileModification(argv, "ObjectStreamsModified") != eSuccess)
         {
         status = eFailure;
         cout<<"failed modification for ObjectStreamsModified\n";            
         }       
         
    } 
    while (false);
    
    return status == eSuccess ? 0 : 1;
    
}

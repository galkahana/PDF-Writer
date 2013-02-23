/*
 Source File : DCTDecodeFilterTest
 
 
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

#include "DCTDecodeFilterTest.h"

#ifndef PDFHUMMUS_NO_DCT

#include "TestsRunner.h"
#include "Trace.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFFormXObject.h"
#include "PageContentContext.h"
#include "PDFImageXObject.h"
#include "PDFParser.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFObject.h"
#include "PDFIndirectObjectReference.h"
#include "PDFStreamInput.h"
#include "PDFArray.h"
#include "PDFDocumentCopyingContext.h"
#include "ObjectsContext.h"
#include "PDFStream.h"
#include "DictionaryContext.h"
#include "OutputStreamTraits.h"

#include <iostream>

using namespace PDFHummus;

DCTDecodeFilterTest::DCTDecodeFilterTest(void)
{
    
}

DCTDecodeFilterTest::~DCTDecodeFilterTest(void)
{
    
}

EStatusCode DCTDecodeFilterTest::Run(const TestConfiguration& inTestConfiguration)
{
    EStatusCode status = eSuccess;
    
    /*
        what we're going to do here is this:
     
        1. create a PDF file using a jpg image. just like the regular jpg image test
        2. open the file for modification
        3. the jpg image should be using DCT decoder. make sure
        4. create a modified image object with the DCT decoding decompressed. 
            4.1 start a new stream
            4.2 copy all but length and decoders details from the original image dictionary
            4.3 create a stream reader for the old image that will decompress the image
            4.4 write it as is to the new image object
        5. now you should have a PDF that has the image decompressed. if it's showing fine...then
           you got a working DCT decoder
     */
    
    do
    {
        string testFileName = "DCTTest.pdf";
        
        // ceate file
        status = CreateFileWithJPGImage(inTestConfiguration,testFileName);
        if(status != eSuccess)
        {
            cout<<"Failed to create jpg file\n";
            break;
        }
        
       
        // open file for modification
        PDFWriter pdfWriter;
        
        status = pdfWriter.ModifyPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,testFileName),ePDFVersion13,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("mod") + testFileName));
		if(status != PDFHummus::eSuccess)
		{
			cout<<"Failed to start PDF for modification\n";
			break;
		}
        
        ObjectIDType imageObject = FindDCTDecodedImageObject(&pdfWriter.GetModifiedFileParser());
        
        if(imageObject == 0)
        {
            status = eFailure;
            cout<<"Image object not found\n";
            break;
        }
        
        status = ModifyImageObject(&pdfWriter,imageObject);
        
        if(status != eSuccess)
        {
            cout<<"Failed to modify PDF\n";
            break;
        }
        
        status = pdfWriter.EndPDF();
        if(status != eSuccess)
        {
            cout<<"Failed to end PDF\n";
            break;
            
        }
    }
    while (false);
    
    return status;
}

EStatusCode DCTDecodeFilterTest::CreateFileWithJPGImage(const TestConfiguration& inTestConfiguration,
                                                        const string& inTestFileName)
{
	PDFWriter pdfWriter;
	EStatusCode status;
    
	do
	{
		status = pdfWriter.StartPDF(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,inTestFileName),ePDFVersion13);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}
        
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));
        
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = PDFHummus::eFailure;
			cout<<"failed to create content context for page\n";
		}
        
		// draw a rectangle
		pageContentContext->q();
		pageContentContext->k(100,0,0,0);
		pageContentContext->re(500,0,100,100);
		pageContentContext->f();
		pageContentContext->Q();
        
		// pause stream to start writing the image
		status = pdfWriter.PausePageContentContext(pageContentContext);
		if(status != PDFHummus::eSuccess)
		{
			cout<<"failed to pause page content context\n";
			break;
		}
        
		// Create image xobject from
		PDFImageXObject* imageXObject  = pdfWriter.CreateImageXObjectFromJPGFile(
                                                                                 RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"TestMaterials/images/otherStage.JPG"));
		if(!imageXObject)
		{
			cout<<"failed to create image XObject from file\n";
			status = PDFHummus::eFailure;
			break;
		}
        
		// continue page drawing size the image to 500,400
		pageContentContext->q();
		pageContentContext->cm(500,0,0,400,0,0);
		pageContentContext->Do(page->GetResourcesDictionary().AddImageXObjectMapping(imageXObject));
		pageContentContext->Q();
        
		delete imageXObject;
        
		status = pdfWriter.EndPageContentContext(pageContentContext);
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

ObjectIDType DCTDecodeFilterTest::FindDCTDecodedImageObject(PDFParser* inParser)
{
    ObjectIDType imageObject = 0;
    
    do
    {
        // find image by looking for the first image in the first page
        
        RefCountPtr<PDFDictionary> firstPage = inParser->ParsePage(0);
        if(!firstPage)
            break;
        
        PDFObjectCastPtr<PDFDictionary> resourceDictionary(inParser->QueryDictionaryObject(firstPage.GetPtr(),"Resources"));
        
        if(!resourceDictionary)
            break;
        
        PDFObjectCastPtr<PDFDictionary> xobjectDictionary(inParser->QueryDictionaryObject(resourceDictionary.GetPtr(), "XObject"));
        
        if(!xobjectDictionary)
            break;
        
        MapIterator<PDFNameToPDFObjectMap> it = xobjectDictionary->GetIterator();

        while(it.MoveNext())
        {
            if(it.GetValue()->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
            {
                PDFObjectCastPtr<PDFStreamInput> image(
                                                       inParser->ParseNewObject(((PDFIndirectObjectReference*)it.GetValue())->mObjectID));
                RefCountPtr<PDFDictionary> imageDictionary = image->QueryStreamDictionary();
                
                PDFObjectCastPtr<PDFName> objectType = imageDictionary->QueryDirectObject("Subtype");
                if(!objectType || objectType->GetValue() != "Image")
                    continue;
                
                RefCountPtr<PDFObject> filters = imageDictionary->QueryDirectObject("Filter");
                if(!filters)
                    break;
                
                if(filters->GetType() == PDFObject::ePDFObjectName &&
                   ((PDFName*)filters.GetPtr())->GetValue() == "DCTDecode")
                {
                    imageObject = ((PDFIndirectObjectReference*)it.GetValue())->mObjectID;
                    break;
                }
                
                PDFArray* filtersArray = (PDFArray*)filters.GetPtr();
                
                if(filtersArray->GetLength() == 1)
                {
                    PDFObjectCastPtr<PDFName> firstDecoder(filtersArray->QueryObject(0));
                    
                    if(firstDecoder->GetValue() == "DCTDecode")
                    {
                        imageObject = ((PDFIndirectObjectReference*)it.GetValue())->mObjectID;
                        break;
                    }
                }
            }
        }
        
    }
    while (false);
    
    return imageObject;
}

EStatusCode DCTDecodeFilterTest::ModifyImageObject(PDFWriter* inWriter,ObjectIDType inImageObject)
{
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* modifiedFileContext = inWriter->CreatePDFCopyingContextForModifiedFile();
    
    do
    {
        // get image source dictionary
        PDFObjectCastPtr<PDFStreamInput> imageStream(inWriter->GetModifiedFileParser().ParseNewObject(inImageObject));
        
        RefCountPtr<PDFDictionary> imageDictionary(imageStream->QueryStreamDictionary());
        
        // strt object for modified image
        inWriter->GetObjectsContext().StartModifiedIndirectObject(inImageObject);
        
        DictionaryContext* newImageDictionary = inWriter->GetObjectsContext().StartDictionary();
        
        MapIterator<PDFNameToPDFObjectMap> it = imageDictionary->GetIterator();
        
        // copy all but "Filter" and "Length"
        ObjectIDTypeList indirectObjects;
        
        while (it.MoveNext())
        {
            if(it.GetKey()->GetValue() == "Filter" || it.GetKey()->GetValue() == "Length")
                continue;
            
            newImageDictionary->WriteKey(it.GetKey()->GetValue());
            EStatusCodeAndObjectIDTypeList result = modifiedFileContext->CopyDirectObjectWithDeepCopy(it.GetValue());
            if(result.first != eSuccess)
            {
                status = result.first;
                break;
            }
            indirectObjects.insert(indirectObjects.end(),result.second.begin(),result.second.end());
        }
        if(status != eSuccess)
            break;
        
        // start image stream for this dictionary (make sure it's unfiltered)
        PDFStream* newImageStream = inWriter->GetObjectsContext().StartUnfilteredPDFStream(newImageDictionary);
        
        // copy source stream through read filter
        IByteReader* sourceImage = modifiedFileContext->GetSourceDocumentParser()->StartReadingFromStream(imageStream.GetPtr());
        if(!sourceImage)
        {
            cout<<"failed to read DCT stream\n";
            status = eFailure;
            break;
        }
        
        OutputStreamTraits traits(newImageStream->GetWriteStream());
        status = traits.CopyToOutputStream(sourceImage);
        
        // finalize stream
        inWriter->GetObjectsContext().EndPDFStream(newImageStream);
        delete newImageStream;
 
        // late check for status so stream is deleted
        if(status != eSuccess)
            break;
        
        // copy remaining indirect objects from image stream dictionary
        status = modifiedFileContext->CopyNewObjectsForDirectObject(indirectObjects);
        
    }
    while (false);
    
    delete modifiedFileContext;
    return status;
    
}


ADD_CATEGORIZED_TEST(DCTDecodeFilterTest,"DCTDecode")

#endif
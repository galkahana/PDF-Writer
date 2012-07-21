/*
 Source File : ModifyingExistingFileContent.cpp
 
 
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
#include "ModifyingExistingFileContent.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFParser.h"
#include "PDFObjectCast.h"
#include "PDFDictionary.h"
#include "PDFDocumentCopyingContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFComment.h"
#include "PDFCommentWriter.h"

#include <iostream>

using namespace PDFHummus;

ModifyingExistingFileContent::ModifyingExistingFileContent()
{
}

ModifyingExistingFileContent::~ModifyingExistingFileContent()
{
    
}

EStatusCode ModifyingExistingFileContent::Run(const TestConfiguration& inTestConfiguration)
{
    
    EStatusCode status = eSuccess;
    PDFWriter pdfWriter;
    
    do 
    {
        
        // Modify existing page. first modify it's page box, then anoteher page content

 		// open file for modification
        status = pdfWriter.ModifyPDF(
                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("TestMaterials/AddedPage.pdf")),
                                     ePDFVersion13,
                                     RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("ModifyingExistingFileContent.pdf")),
                                     LogConfiguration(true,true,RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,string("ModifyingExistingFileContent.log"))));  
		if(status != eSuccess)
		{
			cout<<"failed to start PDF\n";
			break;
		}	        
        
        status = TestPageSizeModification(&pdfWriter);
        if(status != eSuccess)
        {
            cout<<"failed to perform page size modification\n";
            break;
        }
        
        status = TestAddingComments(&pdfWriter);
        if(status != eSuccess)
        {
            cout<<"failed to add comments\n";
            break;
        }
        
		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			cout<<"failed in end PDF\n";
			break;
		}
    }
    while(false);
    
    return status;
}

EStatusCode ModifyingExistingFileContent::TestPageSizeModification(PDFWriter* inPDFWriter)
{
    EStatusCode status = eSuccess;
    PDFDocumentCopyingContext* copyingContext = NULL;
    
    do 
    {
        
        // Change 3rd page bbox to landscape by modifying the page object
        PDFDocumentCopyingContext* copyingContext = inPDFWriter->CreatePDFCopyingContextForModifiedFile();
        
        if(!copyingContext)
        {
            cout<<"failed to create copying context for modified file\n";
            status = eFailure;
            break;
            
        }
        
        // create a new object for the page, copy all but media box, which will be changed
        ObjectIDType thirdPageID = copyingContext->GetSourceDocumentParser()->GetPageObjectID(2);
        PDFObjectCastPtr<PDFDictionary> thirdPageObject = copyingContext->GetSourceDocumentParser()->ParsePage(2);
        
        MapIterator<PDFNameToPDFObjectMap>  thirdPageObjectIt = thirdPageObject->GetIterator();
        
        inPDFWriter->GetObjectsContext().StartModifiedIndirectObject(thirdPageID);
        DictionaryContext* modifiedPageObject = inPDFWriter->GetObjectsContext().StartDictionary();
        
        while(thirdPageObjectIt.MoveNext())
        {
            if(thirdPageObjectIt.GetKey()->GetValue() != "MediaBox")
            {
                modifiedPageObject->WriteKey(thirdPageObjectIt.GetKey()->GetValue());
                copyingContext->CopyDirectObjectAsIs(thirdPageObjectIt.GetValue());
            }
        }   
        
        // write new media box
        modifiedPageObject->WriteKey("MediaBox");
        inPDFWriter->GetObjectsContext().StartArray();
        inPDFWriter->GetObjectsContext().WriteInteger(0);
        inPDFWriter->GetObjectsContext().WriteInteger(0);
        inPDFWriter->GetObjectsContext().WriteInteger(500);
        inPDFWriter->GetObjectsContext().WriteInteger(500);
        inPDFWriter->GetObjectsContext().EndArray();
        inPDFWriter->GetObjectsContext().EndLine();
        
        inPDFWriter->GetObjectsContext().EndDictionary(modifiedPageObject);
        inPDFWriter->GetObjectsContext().EndIndirectObject();
        
        // done changing page box (ok...that seemed like a lot of work for this...humff)
        
    }
    while(false);
    
    // cleanup
    delete copyingContext;
    
    
    return status;
}

EStatusCode ModifyingExistingFileContent::TestAddingComments(PDFWriter* inPDFWriter)
{
    EStatusCode status = eSuccess;
    PDFComment* aComment = NULL;
    PDFComment* bComment = NULL;
    PDFComment* cComment = NULL;
    PDFDocumentCopyingContext* copyingContext = NULL;
    
    do
    {
        PDFCommentToObjectIDTypeMap pageCommentContext;
        
        PDFCommentWriter commentWriter(inPDFWriter);
        
		aComment = new PDFComment();
        
		aComment->Text = "a very important comment";
		aComment->CommentatorName = "Someone";
		aComment->FrameBoundings[0] = 100;
		aComment->FrameBoundings[1] = 500;
		aComment->FrameBoundings[2] = 200;
		aComment->FrameBoundings[3] = 600;
		aComment->Color = CMYKRGBColor(255,0,0);
        
        status = commentWriter.WriteCommentTree(aComment,pageCommentContext);
		if(status != eSuccess)
        {
            status = eFailure;
            cout<<"can't write first comment";
			break;
        }
        
		bComment = new PDFComment();
        
		bComment->Text = "I have nothing to say about this";
		bComment->CommentatorName = "Someone";
		bComment->FrameBoundings[0] = 100;
		bComment->FrameBoundings[1] = 100;
		bComment->FrameBoundings[2] = 200;
		bComment->FrameBoundings[3] = 200;
		bComment->Color = CMYKRGBColor(255,0,0);
                
		cComment = new PDFComment();
        
		cComment->Text = "yeah. me too. it's just perfect";
		cComment->CommentatorName = "Someone Else";
		cComment->FrameBoundings[0] = 150;
		cComment->FrameBoundings[1] = 150;
		cComment->FrameBoundings[2] = 250;
		cComment->FrameBoundings[3] = 250;
		cComment->Color = CMYKRGBColor(0,255,0);
		cComment->ReplyTo = bComment;
		
        status = commentWriter.WriteCommentTree(cComment,pageCommentContext);
		if(status != eSuccess)
        {
            status = eFailure;
            cout<<"can't write second and third comment";
			break;
        }

        // now write these comments to the 4th page
        PDFDocumentCopyingContext* copyingContext = inPDFWriter->CreatePDFCopyingContextForModifiedFile();
        
        if(!copyingContext)
        {
            cout<<"failed to create copying context for modified file\n";
            status = eFailure;
            break;
            
        }
        
        // create a new object for the page, copy all but annotations, which will be changed
        ObjectIDType fourthPageID = copyingContext->GetSourceDocumentParser()->GetPageObjectID(3);
        PDFObjectCastPtr<PDFDictionary> fourthPageObject = copyingContext->GetSourceDocumentParser()->ParsePage(3);
        
        MapIterator<PDFNameToPDFObjectMap>  fourthPageObjectIt = fourthPageObject->GetIterator();
        
        inPDFWriter->GetObjectsContext().StartModifiedIndirectObject(fourthPageID);
        DictionaryContext* modifiedPageObject = inPDFWriter->GetObjectsContext().StartDictionary();
        
        while(fourthPageObjectIt.MoveNext())
        {
            if(fourthPageObjectIt.GetKey()->GetValue() != "Annots")
            {
                modifiedPageObject->WriteKey(fourthPageObjectIt.GetKey()->GetValue());
                copyingContext->CopyDirectObjectAsIs(fourthPageObjectIt.GetValue());
            }
        }     
        
        // write new annotations array with comments
        modifiedPageObject->WriteKey("Annots");
        inPDFWriter->GetObjectsContext().StartArray();

        PDFCommentToObjectIDTypeMap::iterator it = pageCommentContext.begin();
        for(; it != pageCommentContext.end(); ++it)
            inPDFWriter->GetObjectsContext().WriteIndirectObjectReference(it->second);
        
        inPDFWriter->GetObjectsContext().EndArray(eTokenSeparatorEndLine);
        
        inPDFWriter->GetObjectsContext().EndDictionary(modifiedPageObject);
        inPDFWriter->GetObjectsContext().EndIndirectObject();
    }
    while(false);
    
    delete aComment;
    delete bComment;
    delete cComment;
    delete copyingContext;
    return status;
}

ADD_CATEGORIZED_TEST(ModifyingExistingFileContent,"Modification")



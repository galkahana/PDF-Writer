/*
   Source File : SweepGradientShadingPatternWritingTask.cpp


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


#include "SweepGradientShadingPatternWritingTask.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTiledPattern.h"
#include "PDFFormXObject.h"
#include "TiledPatternContentContext.h"
#include "XObjectContentContext.h"
#include "PDFStream.h"

using namespace PDFHummus;

SweepGradientShadingPatternWritingTask::SweepGradientShadingPatternWritingTask(
    double inCX,
    double inCY,
    double inStartAngleRad,
    double inEndAngleRad,
    InterpretedGradientStopList inColorLine,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
):AbstractGradientShadingPatternWritingTask(inColorLine, inBounds, inMatrix, inPatternObjectId) {
    cX = inCX;
    cY = inCY;
    startAngleRad = inStartAngleRad;
    endAngleRad = inEndAngleRad;
}

SweepGradientShadingPatternWritingTask::~SweepGradientShadingPatternWritingTask(){
}

EStatusCode SweepGradientShadingPatternWritingTask::WriteRGBShadingPatternObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
    EStatusCode status = eSuccess;
    PDFStream* pdfStream = NULL;

    ObjectIDType functionObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(); 

    PDFMatrix patternProgramMatrix = PDFMatrix(1,0,0,1,cX,cY);

    PDFMatrix patternMatrix = patternProgramMatrix.Multiply(matrix);
    PDFRectangle domainBounds = patternProgramMatrix.Inverse().Transform(bounds);

    do {
        // shading object
        inObjectsContext->StartNewIndirectObject(inObjectID);
        DictionaryContext* patternDict = inObjectsContext->StartDictionary();
        patternDict->WriteKey("Type");
        patternDict->WriteNameValue("Pattern");
        patternDict->WriteKey("PatternType");
        patternDict->WriteIntegerValue(2);
        patternDict->WriteKey("Matrix");
        inObjectsContext->StartArray();
        inObjectsContext->WriteDouble(patternMatrix.a);
        inObjectsContext->WriteDouble(patternMatrix.b);
        inObjectsContext->WriteDouble(patternMatrix.c);
        inObjectsContext->WriteDouble(patternMatrix.d);
        inObjectsContext->WriteDouble(patternMatrix.e);
        inObjectsContext->WriteDouble(patternMatrix.f);
        inObjectsContext->EndArray(eTokenSeparatorEndLine);
        patternDict->WriteKey("Shading");
        DictionaryContext* shadingDict = inObjectsContext->StartDictionary();
        shadingDict->WriteKey("ShadingType");
        shadingDict->WriteIntegerValue(1);
        shadingDict->WriteKey("Domain");
        inObjectsContext->StartArray();
        inObjectsContext->WriteDouble(domainBounds.LowerLeftX);
        inObjectsContext->WriteDouble(domainBounds.UpperRightX);
        inObjectsContext->WriteDouble(domainBounds.LowerLeftY);
        inObjectsContext->WriteDouble(domainBounds.UpperRightY);
        inObjectsContext->EndArray(eTokenSeparatorEndLine);    
        shadingDict->WriteKey("ColorSpace");
        shadingDict->WriteNameValue("DeviceRGB");
        shadingDict->WriteKey("Function");
        shadingDict->WriteNewObjectReferenceValue(functionObjectId);
        status = inObjectsContext->EndDictionary(shadingDict);
        if(status != eSuccess)
            break;
        status = inObjectsContext->EndDictionary(patternDict);
        if(status != eSuccess)
            break;
        inObjectsContext->EndIndirectObject();


        // function object
        inObjectsContext->StartNewIndirectObject(functionObjectId);
        DictionaryContext* functionDict = inObjectsContext->StartDictionary();
        functionDict->WriteKey("FunctionType");
        functionDict->WriteIntegerValue(4);
        functionDict->WriteKey("Domain");
        inObjectsContext->StartArray();
        inObjectsContext->WriteDouble(domainBounds.LowerLeftX);
        inObjectsContext->WriteDouble(domainBounds.UpperRightX);
        inObjectsContext->WriteDouble(domainBounds.LowerLeftY);
        inObjectsContext->WriteDouble(domainBounds.UpperRightY);
        inObjectsContext->EndArray(eTokenSeparatorEndLine);    
        functionDict->WriteKey("Range");
        inObjectsContext->StartArray();
        inObjectsContext->WriteInteger(0);
        inObjectsContext->WriteInteger(1);
        inObjectsContext->WriteInteger(0);
        inObjectsContext->WriteInteger(1);
        inObjectsContext->WriteInteger(0);
        inObjectsContext->WriteInteger(1);
        inObjectsContext->EndArray(eTokenSeparatorEndLine);            
        
        // now lets make this a stream so we can wrie the Postscript code
        PDFStream* pdfStream = inObjectsContext->StartPDFStream(functionDict);
        if(!pdfStream) {
            status = eFailure;
            break;
        }

        mWriteStream = pdfStream->GetWriteStream();
        mPrimitiveWriter.SetStreamForWriting(mWriteStream);
        WriteGradientProgram(inRGBColorLine);
        inObjectsContext->EndPDFStream(pdfStream);
    }while(false);
    delete pdfStream;

    return status;
}

void SweepGradientShadingPatternWritingTask::WriteGradientProgram(const InterpretedGradientStopList& inRGBColorLine) {
    // write postscript code to define the function. thank you Skia :)

    // function start, translate t to step per sweep function
    WriteStreamText("{exch atan 360 div\n");
    
    // write the rest of the steps using shared code
    WriteColorLineStepsProgram(inRGBColorLine);

    // function end
    WriteStreamText("}");    
}
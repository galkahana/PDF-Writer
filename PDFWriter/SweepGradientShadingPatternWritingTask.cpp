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

    PDFMatrix patternMatrix = PDFMatrix(1,0,0,1,cX,cY).Multiply(matrix);
    PDFRectangle domainBounds = PDFMatrix(1,0,0,1,-cX,-cY).Transform(bounds); // haha there's a simply way here

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
        double firstStop = inRGBColorLine.front().stopOffset;
        double lastStop = inRGBColorLine.back().stopOffset;
        // write encodes for size-1 functions
        for(int i=0; i<inRGBColorLine.size()-1;++i) {
            inObjectsContext->WriteDouble(firstStop);
            inObjectsContext->WriteDouble(lastStop);
        }
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
    double firstStop = inRGBColorLine.front().stopOffset;
    double lastStop = inRGBColorLine.back().stopOffset;

    // write postscript code to define the function. thank you Skia :)

    // function start
    WriteStreamText("{exch atan 360 div\n");
    
    // padding unto first stop
    WriteStreamText("dup ");
    mPrimitiveWriter.WriteDouble(firstStop);
    WriteStreamText("le {pop ");
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.red / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.green / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.front().color.blue / 255.0);
    WriteStreamText("0} if\n");        

    // wrapper for in between stops code
    WriteStreamText("dup dup ");
    mPrimitiveWriter.WriteDouble(firstStop);
    WriteStreamText("gt exch ");
    mPrimitiveWriter.WriteDouble(lastStop);
    WriteStreamText("le and ");

    // k. interim stops, lets prep those cursers
    InterpretedGradientStopList::const_iterator itStartRange = inRGBColorLine.begin();
    InterpretedGradientStopList::const_iterator itEndRange = inRGBColorLine.begin();
    ++itEndRange;

    // k. write stops, and interpolate.
    for(; itEndRange != inRGBColorLine.end(); ++itStartRange,++itEndRange) {
        if(itStartRange->stopOffset > firstStop) {
            // all but first, close previous if of ifelse statement
            WriteStreamText("}");
        }
        WriteStreamText("{\n");
        if(itEndRange->stopOffset < lastStop) {
            // all but last check value (last will enjoy the last else of ifelse)
            WriteStreamText("dup ");
            mPrimitiveWriter.WriteDouble(itEndRange->stopOffset);
            WriteStreamText("le ");
            WriteStreamText("{\n");
        }
        mPrimitiveWriter.WriteDouble(itStartRange->stopOffset);
        WriteStreamText("sub ");

        // if all colors are the same, just write as is

        // otherwise, interpolate
        if(itStartRange->color.red == itEndRange->color.red &&
            itStartRange->color.green == itEndRange->color.green &&
            itStartRange->color.blue == itEndRange->color.blue) {

            WriteStreamText("pop ");
            mPrimitiveWriter.WriteDouble(itEndRange->color.red / 255.0);
            mPrimitiveWriter.WriteDouble(itEndRange->color.green / 255.0);
            mPrimitiveWriter.WriteDouble(itEndRange->color.blue / 255.0);
        } else {
            double rangeDiff = itEndRange->stopOffset - itStartRange->stopOffset;
            WriteColorInterpolation(itStartRange->color.red, itEndRange->color.red, rangeDiff);
            WriteColorInterpolation(itStartRange->color.green, itEndRange->color.green, rangeDiff);
            WriteColorInterpolation(itStartRange->color.blue, itEndRange->color.blue, rangeDiff);
            WriteStreamText("pop ");
        }
    }

    // "ifselse" (basically size -2, cause only all but first, and then always dual)
    for(int i=0;i<inRGBColorLine.size()-2;++i) {
        WriteStreamText("} ifelse\n");
    }

    WriteStreamText("0} if\n");

    // padding after last stop
    WriteStreamText("0 gt {");
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.red / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.green / 255.0);
    mPrimitiveWriter.WriteDouble(inRGBColorLine.back().color.blue / 255.0);
    WriteStreamText("} if\n");


    // function end
    WriteStreamText("}");    
}

void SweepGradientShadingPatternWritingTask::WriteColorInterpolation(FT_Byte inColorStart, FT_Byte inColorEnd, double inStopDiff) {
    if(inColorEnd == inColorStart) {
        // same color...so just put it there
        mPrimitiveWriter.WriteDouble(inColorEnd/255.0);
    } else {
        // interpolate
        WriteStreamText("dup ");
        mPrimitiveWriter.WriteDouble((inColorEnd - inColorStart)/(inStopDiff*255.0));
        WriteStreamText("mul ");
        mPrimitiveWriter.WriteDouble(inColorStart/255.0);
        WriteStreamText("add ");
    }
    WriteStreamText("exch ");
}

void SweepGradientShadingPatternWritingTask::WriteStreamText(std::string inString) {
    mWriteStream->Write((const Byte*)(inString.c_str()),inString.size());
}
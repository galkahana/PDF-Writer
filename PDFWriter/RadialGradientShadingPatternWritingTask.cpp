/*
   Source File : RadialGlyphsDrawingContext.cpp


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


#include "RadialGradientShadingPatternWritingTask.h"
#include "DocumentContext.h"
#include "ObjectsContext.h"
#include "DictionaryContext.h"
#include "PDFTiledPattern.h"
#include "PDFFormXObject.h"
#include "TiledPatternContentContext.h"
#include "XObjectContentContext.h"
#include "PDFStream.h"
#include <math.h>

using namespace PDFHummus;

RadialGradientShadingPatternWritingTask::RadialGradientShadingPatternWritingTask(
    double inX0,
    double inY0,
    double inR0,
    double inX1,
    double inY1,
    double inR1,
    InterpretedGradientStopList inColorLine,
    FT_PaintExtend inGradientExtend,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
):AbstractGradientShadingPatternWritingTask(inColorLine, inBounds, inMatrix, inPatternObjectId) {
   x0 = inX0;
   y0 = inY0;
   r0 = inR0;
   x1 = inX1;
   y1 = inY1;
   r1 = inR1;
   mGradientExtend = inGradientExtend;
}

RadialGradientShadingPatternWritingTask::~RadialGradientShadingPatternWritingTask(){
}

EStatusCode RadialGradientShadingPatternWritingTask::WriteRGBShadingPatternObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {

    if(mGradientExtend == FT_COLR_PAINT_EXTEND_PAD) {
        return WriteNativePDFRadialShadingPatternObject(
            inRGBColorLine,
            inObjectID,
            inObjectsContext,
            inDocumentContext
        );
    } else {
        return WriteRadialShadingPatternWithFunctionObject(
            inRGBColorLine,
            inObjectID,
            inObjectsContext,
            inDocumentContext
        );
    }
}

EStatusCode RadialGradientShadingPatternWritingTask::WriteNativePDFRadialShadingPatternObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
        EStatusCode status = eSuccess;

        do {
            inObjectsContext->StartNewIndirectObject(inObjectID);
            DictionaryContext* patternDict = inObjectsContext->StartDictionary();
            patternDict->WriteKey("Type");
            patternDict->WriteNameValue("Pattern");
            patternDict->WriteKey("PatternType");
            patternDict->WriteIntegerValue(2);
            patternDict->WriteKey("Matrix");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(matrix.a);
            inObjectsContext->WriteDouble(matrix.b);
            inObjectsContext->WriteDouble(matrix.c);
            inObjectsContext->WriteDouble(matrix.d);
            inObjectsContext->WriteDouble(matrix.e);
            inObjectsContext->WriteDouble(matrix.f);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            patternDict->WriteKey("Shading");
            DictionaryContext* shadingDict = inObjectsContext->StartDictionary();
            shadingDict->WriteKey("ShadingType");
            shadingDict->WriteIntegerValue(3);
            shadingDict->WriteKey("Coords");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(x0);
            inObjectsContext->WriteDouble(y0);
            inObjectsContext->WriteDouble(r0);
            inObjectsContext->WriteDouble(x1);
            inObjectsContext->WriteDouble(y1);
            inObjectsContext->WriteDouble(r1);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);    
            shadingDict->WriteKey("ColorSpace");
            shadingDict->WriteNameValue("DeviceRGB");
            shadingDict->WriteKey("Extend");
            inObjectsContext->StartArray();
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->WriteBoolean(true);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            shadingDict->WriteKey("Function");
            DictionaryContext* functionDict = inObjectsContext->StartDictionary();
            functionDict->WriteKey("FunctionType");
            functionDict->WriteIntegerValue(3);
            functionDict->WriteKey("Bounds");
            inObjectsContext->StartArray();
            // bound contains whats within the edges, so should skip
            // first and last (which are expected to be 0 and 1)
            InterpretedGradientStopList::const_iterator it1 = inRGBColorLine.begin();
            InterpretedGradientStopList::const_iterator it2 = inRGBColorLine.begin();
            ++it1;
            ++it2;
            ++it1;
            for(; it1 != inRGBColorLine.end();++it1,++it2) {
                inObjectsContext->WriteDouble(it2->stopOffset);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            // last stop, which should probably be 1, is saved cause will be used in domains
            double lastStop = it2->stopOffset;
            functionDict->WriteKey("Domain");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(0);
            inObjectsContext->WriteDouble(1);
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Encode");
            inObjectsContext->StartArray();
            // write encodes for size-1 functions
            for(int i=0; i<inRGBColorLine.size()-1;++i) {
                inObjectsContext->WriteDouble(0);
                inObjectsContext->WriteDouble(1);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            functionDict->WriteKey("Functions");
            // draw all stops, each in its own function. note that there should be size-1 functions!
            inObjectsContext->StartArray();
            it1 = inRGBColorLine.begin();
            it2 = inRGBColorLine.begin();
            ++it1;
            for(; it1 != inRGBColorLine.end() && eSuccess == status;++it1,++it2) {
                DictionaryContext* colorStopDict = inObjectsContext->StartDictionary();
                colorStopDict->WriteKey("FunctionType");
                colorStopDict->WriteIntegerValue(2);
                colorStopDict->WriteKey("N");
                colorStopDict->WriteIntegerValue(1);
                colorStopDict->WriteKey("Domain");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(0);
                inObjectsContext->WriteDouble(lastStop);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C0");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(it2->color.red/255.0);
                inObjectsContext->WriteDouble(it2->color.green/255.0);
                inObjectsContext->WriteDouble(it2->color.blue/255.0);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                colorStopDict->WriteKey("C1");
                inObjectsContext->StartArray();
                inObjectsContext->WriteDouble(it1->color.red/255.0);
                inObjectsContext->WriteDouble(it1->color.green/255.0);
                inObjectsContext->WriteDouble(it1->color.blue/255.0);
                inObjectsContext->EndArray(eTokenSeparatorEndLine);
                status = inObjectsContext->EndDictionary(colorStopDict);
                if(status != eSuccess)
                    break;
            } 
            if(status != eSuccess)
                break;

            inObjectsContext->EndArray(eTokenSeparatorEndLine);
            status = inObjectsContext->EndDictionary(functionDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(shadingDict);
            if(status != eSuccess)
                break;
            status = inObjectsContext->EndDictionary(patternDict);
            if(status != eSuccess)
                break;
            inObjectsContext->EndIndirectObject();
        }while(false);
        return status;
}

EStatusCode RadialGradientShadingPatternWritingTask::WriteRadialShadingPatternWithFunctionObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
    EStatusCode status = eSuccess;
    PDFStream* pdfStream = NULL;

    ObjectIDType functionObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(); 

    // apply transform matrix, to map pattern based on function range
    PDFMatrix patternProgramMatrix(1,0,0,1,x0,y0);

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
        WriteGradientFunctionProgram(inRGBColorLine);
        inObjectsContext->EndPDFStream(pdfStream);
    }while(false);
    delete pdfStream;

    return status;
}

void RadialGradientShadingPatternWritingTask::WriteGradientFunctionProgram(const InterpretedGradientStopList& inRGBColorLine) {
    // write postscript code to define the function. thank you Skia :)
    double dx = x1-x0;
    double dy = y1-y0;
    double dr = r1-r0;
    double a = dx * dx + dy * dy - dr * dr;

    WriteStreamText("{2 copy ");
    mPrimitiveWriter.WriteDouble(dy);
    WriteStreamText("mul exch ");
    mPrimitiveWriter.WriteDouble(dx);
    WriteStreamText("mul add ");
    mPrimitiveWriter.WriteDouble(r0*dr);        
    WriteStreamText(" add -2 mul dup dup mul\n4 2 roll dup mul exch dup mul add ");
    mPrimitiveWriter.WriteDouble(r0*r0);        
    WriteStreamText("sub dup 4 1 roll\n");

    if(a == 0) {
        WriteStreamText("pop pop div neg dup ");
        mPrimitiveWriter.WriteDouble(dr);
        WriteStreamText("mul ");
        mPrimitiveWriter.WriteDouble(r0);
        WriteStreamText("add\n0 lt {pop false} {true} ifelse\n");
    } else {
        mPrimitiveWriter.WriteDouble(a*4);  
        WriteStreamText("mul sub dup\n0 ge {\nsqrt exch dup 0 lt {exch -1 mul} if add -0.5 mul dup\n");
        mPrimitiveWriter.WriteDouble(a);  
        WriteStreamText("div\n3 1 roll div\n2 copy gt {exch} if\ndup ");
        mPrimitiveWriter.WriteDouble(dr);  
        WriteStreamText("mul ");
        mPrimitiveWriter.WriteDouble(r0);
        WriteStreamText("add\n 0 gt {exch pop true}\n{pop dup\n");
        mPrimitiveWriter.WriteDouble(dr);  
        WriteStreamText("mul ");
        mPrimitiveWriter.WriteDouble(r0);
        WriteStreamText("add\n");
        WriteStreamText("0 le {pop false} {true} ifelse\n} ifelse\n} {pop pop pop false} ifelse\n");
    }

    WriteStreamText("{");
    
    // function start, setup header per extend policy
    if(mGradientExtend == FT_COLR_PAINT_EXTEND_REPEAT)
        WriteStreamText("dup truncate sub\ndup 0 le {1 add} if\n");
    else // FT_COLR_PAINT_EXTEND_REFLECT
        WriteStreamText("abs dup truncate dup cvi 2 mod 0 gt 3 1 roll sub exch {1 exch sub} if\n");
    
    // write the rest of the steps using shared code
    WriteColorLineStepsProgram(inRGBColorLine);

    WriteStreamText("} {0 0 0} ifelse}");

}
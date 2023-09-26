/*
   Source File : LinearGradientShadingPatternWritingTask.cpp


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


#include "LinearGradientShadingPatternWritingTask.h"
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

LinearGradientShadingPatternWritingTask::LinearGradientShadingPatternWritingTask(
    double inX0,
    double inY0,
    double inX1,
    double inY1,
    InterpretedGradientStopList inColorLine,
    FT_PaintExtend inGradientExtend,
    PDFRectangle inBounds,
    PDFMatrix inMatrix,
    ObjectIDType inPatternObjectId
):AbstractGradientShadingPatternWritingTask(inColorLine, inBounds, inMatrix, inPatternObjectId) {
   x0 = inX0;
   y0 = inY0;
   x1 = inX1;
   y1 = inY1;
   mGradientExtend = inGradientExtend;
}

LinearGradientShadingPatternWritingTask::~LinearGradientShadingPatternWritingTask(){
}

EStatusCode LinearGradientShadingPatternWritingTask::WriteRGBShadingPatternObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {

    if(mGradientExtend == FT_COLR_PAINT_EXTEND_PAD) {
        return WriteNativePDFLinearShadingPatternObject(
            inRGBColorLine,
            inObjectID,
            inObjectsContext,
            inDocumentContext
        );
    } else {
        return WriteLinearShadingPatternWithFunctionObject(
            inRGBColorLine,
            inObjectID,
            inObjectsContext,
            inDocumentContext
        );
    }
}

EStatusCode LinearGradientShadingPatternWritingTask::WriteNativePDFLinearShadingPatternObject(
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
            shadingDict->WriteIntegerValue(2);
            shadingDict->WriteKey("Coords");
            inObjectsContext->StartArray();
            inObjectsContext->WriteDouble(x0);
            inObjectsContext->WriteDouble(y0);
            inObjectsContext->WriteDouble(x1);
            inObjectsContext->WriteDouble(y1);
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
            // first and last
            InterpretedGradientStopList::const_iterator it1 = inRGBColorLine.begin();
            InterpretedGradientStopList::const_iterator it2 = inRGBColorLine.begin();
            ++it1;
            ++it2;
            ++it1;
            for(; it1 != inRGBColorLine.end();++it1,++it2) {
                inObjectsContext->WriteDouble(it2->stopOffset);
            }
            inObjectsContext->EndArray(eTokenSeparatorEndLine);
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
                inObjectsContext->WriteDouble(1);
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

EStatusCode LinearGradientShadingPatternWritingTask::WriteLinearShadingPatternWithFunctionObject(
    const InterpretedGradientStopList& inRGBColorLine,
    ObjectIDType inObjectID, 
    ObjectsContext* inObjectsContext,
    DocumentContext* inDocumentContext) {
    EStatusCode status = eSuccess;
    PDFStream* pdfStream = NULL;

    ObjectIDType functionObjectId = inObjectsContext->GetInDirectObjectsRegistry().AllocateNewObjectID(); 

    // apply transform matrix, to map pattern based on function range
    double p0p1[2] = {x1-x0,y1-y0};
    double magP0P1 = sqrt(pow(p0p1[0],2) + pow(p0p1[1],2));
    double p0p1Unit[2] = {magP0P1 == 0 ? 0:p0p1[0]/magP0P1, magP0P1 == 0 ? 0:p0p1[1]/magP0P1};
    PDFMatrix patternProgramMatrix(p0p1Unit[0],p0p1Unit[1],-p0p1Unit[1],p0p1Unit[0],0,0);
    patternProgramMatrix = patternProgramMatrix.Multiply(PDFMatrix(magP0P1,0,0,magP0P1,0,0));
    patternProgramMatrix.e+=x0;
    patternProgramMatrix.f+=y0;

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

void LinearGradientShadingPatternWritingTask::WriteGradientFunctionProgram(const InterpretedGradientStopList& inRGBColorLine) {
    // write postscript code to define the function. thank you Skia :)

    WriteStreamText("{pop\n");
    // function start, setup header per extend policy
    if(mGradientExtend == FT_COLR_PAINT_EXTEND_REPEAT)
        WriteStreamText("dup truncate sub\ndup 0 le {1 add} if\n");
    else // FT_COLR_PAINT_EXTEND_REFLECT
        WriteStreamText("abs dup truncate dup cvi 2 mod 0 gt 3 1 roll sub exch {1 exch sub} if\n");
    
    // write the rest of the steps using shared code
    WriteColorLineStepsProgram(inRGBColorLine);

    // function end
    WriteStreamText("}");   
}

/*
   Source File : AbstractGradientShadingPatternWritingTask.h


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

#pragma once

#include "InterpretedGradientStop.h"
#include "EStatusCode.h"
#include "IObjectEndWritingTask.h"
#include "ObjectsBasicTypes.h"
#include "PDFRectangle.h"
#include "PDFMatrix.h"
#include "PrimitiveObjectsWriter.h"

#include <string>

class DictionaryContext;
class IByteWriter;
class AbstractGradientShadingPatternWritingTask: public IObjectEndWritingTask {
    public:
        AbstractGradientShadingPatternWritingTask(
            InterpretedGradientStopList inColorLine,
            PDFRectangle inBounds,
            PDFMatrix inMatrix,
            ObjectIDType inPatternObjectId
        );

        virtual ~AbstractGradientShadingPatternWritingTask();

        virtual PDFHummus::EStatusCode Write(ObjectsContext* inObjectsContext,
                                            PDFHummus::DocumentContext* inDocumentContext);
    protected:
        PDFRectangle bounds;
        PDFMatrix matrix;

        // reusable members+functions for writing shared code of gradient postscript function

        // make sure to setup those in your WriteRGBShadingPatternObject version for the functions below to work
        IByteWriter* mWriteStream;
        PrimitiveObjectsWriter mPrimitiveWriter;

        void WriteColorLineStepsProgram(const InterpretedGradientStopList& inRGBColorLine);
        void WriteStreamText(const std::string& inString);

    private:
        // override to write implementation specific shading pattern details
        virtual PDFHummus::EStatusCode WriteRGBShadingPatternObject(const InterpretedGradientStopList& inColorLine, ObjectIDType inObjectID, ObjectsContext* inObjectsContext, PDFHummus::DocumentContext* inDocumentContext) = 0;

        InterpretedGradientStopList colorLine;
        ObjectIDType patternObjectId;

        bool ColorLineHasTransparency();
        PDFHummus::EStatusCode WriteRGBATiledPatternObject(ObjectIDType inObjectID, ObjectsContext* inObjectsContext, PDFHummus::DocumentContext* inDocumentContext);
        PDFHummus::EStatusCode WriteAlphaSoftMaskExtGState(ObjectIDType inObjectID, ObjectsContext* inObjectsContext, PDFHummus::DocumentContext* inDocumentContext);
        PDFHummus::EStatusCode WriteAlphaSoftMaskForm(ObjectIDType inObjectID, ObjectsContext* inObjectsContext, PDFHummus::DocumentContext* inDocumentContext);
        void WriteColorInterpolation(FT_Byte inColorStart, FT_Byte inColorEnd, double inStopDiff);
};

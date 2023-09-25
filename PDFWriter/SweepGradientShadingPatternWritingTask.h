/*
   Source File : SweepGradientShadingPatternWritingTask.h


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
#include "AbstractGradientShadingPatternWritingTask.h"
#include "PrimitiveObjectsWriter.h"

#include <string>

class IByteWriter;

class SweepGradientShadingPatternWritingTask: public AbstractGradientShadingPatternWritingTask {
    public:
        SweepGradientShadingPatternWritingTask(
            double inCX,
            double inCY,
            double inStartAngleRad,
            double inEndAngleRad,
            InterpretedGradientStopList inColorLine,
            PDFRectangle inBounds,
            PDFMatrix inMatrix,
            ObjectIDType inPatternObjectId
        );

        virtual ~SweepGradientShadingPatternWritingTask();

    private:
        virtual PDFHummus::EStatusCode WriteRGBShadingPatternObject(const InterpretedGradientStopList& inColorLine, ObjectIDType inObjectID, ObjectsContext* inObjectsContext, PDFHummus::DocumentContext* inDocumentContext);

        double cX;
        double cY;
        double startAngleRad;
        double endAngleRad;

        IByteWriter* mWriteStream;
        PrimitiveObjectsWriter mPrimitiveWriter;

        void WriteGradientProgram(const InterpretedGradientStopList& inColorLine);
        void WriteColorInterpolation(FT_Byte inColorStart, FT_Byte inColorEnd, double inStopDiff);
        void WriteStreamText(std::string inString);
};

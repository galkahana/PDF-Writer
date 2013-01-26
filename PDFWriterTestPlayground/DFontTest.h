//
//  DFontTest.h
//  PDFWriterTestPlayground
//
//  Created by Gal Kahana on 1/26/13.
//  Copyright (c) 2013 gal.kahana@hotmail.com. All rights reserved.
//

#ifndef __PDFWriterTestPlayground__DFontTest__
#define __PDFWriterTestPlayground__DFontTest__

#include <iostream>

#endif /* defined(__PDFWriterTestPlayground__DFontTest__) */
/*
 Source File : DFontTest.h
 
 
 Copyright 2013 Gal Kahana PDFWriter
 
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
#include "ITestUnit.h"


class DFontTest : public ITestUnit
{
public:
	DFontTest(void);
	virtual ~DFontTest(void);
    
	virtual PDFHummus::EStatusCode Run(const TestConfiguration& inTestConfiguration);
    
};
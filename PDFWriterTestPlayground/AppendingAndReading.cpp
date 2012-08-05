/*
 Source File : AppendingAndReading.cpp
 
 
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
#include "AppendingAndReading.h"
#include "TestsRunner.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "IByteWriterWithPosition.h"
#include "IByteReaderWithPosition.h"

#include <iostream>

using namespace PDFHummus;

AppendingAndReading::AppendingAndReading()
{
}

AppendingAndReading::~AppendingAndReading()
{
    
}

static const Byte scTest[5] = {'T','e','s','t','\n'};
static const Byte scTest1[6] = {'T','e','s','t','1','\n'};
static const Byte scTest2[6] = {'T','e','s','t','2','\n'};

EStatusCode AppendingAndReading::Run(const TestConfiguration& inTestConfiguration)
{
    // checks if i can create a file, and then both read and write to it at the same
    // time with different classes. The test aims only for the scenario of reading from the part
    // that is not written in, as would be the case for PDFs with incremental updates
    
    EStatusCode status = eSuccess;
    
    do 
    {
        // write initial file
        {
            OutputFile outputFile;
            
            status = outputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ReadAndWrite.txt"));
            if(status != eSuccess)
            {
                cout<<"Failed to open file for writing";
                break;
                
            }
            
            
            outputFile.GetOutputStream()->Write(scTest,5);
            outputFile.GetOutputStream()->Write(scTest1,6);
            outputFile.GetOutputStream()->Write(scTest2,6);
        }
    
        // now open file for read and file for append
        
        InputFile inputFile;
        
        status = inputFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ReadAndWrite.txt"));
        if(status != eSuccess)
        {
            cout<<"Failed to open file for reading\n";
            status = eFailure;
            break;
            
        }
        
        OutputFile appendToFile;
        
        status = appendToFile.OpenFile(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase,"ReadAndWrite.txt"),true);
        if(status != eSuccess)
        {
            cout<<"Failed to open file for appending\n";
            status = eFailure;
            break;
            
        }
        
        // check reading 1
        inputFile.GetInputStream()->SetPosition(5);
        Byte bufferTest1[6];
        
        inputFile.GetInputStream()->Read(bufferTest1,6);
        if(memcmp(bufferTest1,scTest1,6) != 0)
        {
            cout<<"failed comparison in reading 1\n";
            status = eFailure;
            break;           
        }
        
        
        // check writing 1
        appendToFile.GetOutputStream()->Write(scTest1,6);
        
        // check reading 2
        inputFile.GetInputStream()->Read(bufferTest1,6);
        if(memcmp(bufferTest1,scTest2,6) != 0)
        {
            cout<<"failed comparison in reading 2\n";
            status = eFailure;
            break;           
        }
        
        // check reading 3
        inputFile.GetInputStream()->SetPosition(0);
        Byte bufferTest[5];
        inputFile.GetInputStream()->Read(bufferTest,5);
        if(memcmp(bufferTest,scTest,5) != 0)
        {
            cout<<"failed comparison in reading 3\n";
            status = eFailure;
            break;           
        }
      
        // check writing 2
        appendToFile.GetOutputStream()->Write(scTest1,6);
        
    } 
    while(false);
 
    return status;
}

ADD_CATEGORIZED_TEST(AppendingAndReading,"Modification")



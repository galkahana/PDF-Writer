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
#include "InputFile.h"
#include "OutputFile.h"
#include "IByteWriterWithPosition.h"
#include "IByteReaderWithPosition.h"

#include "testing/TestIO.h"

#include <iostream>
#include <memory.h>

using namespace PDFHummus;
using namespace std;


static const Byte scTest[5] = {'T','e','s','t','\n'};
static const Byte scTest1[6] = {'T','e','s','t','1','\n'};
static const Byte scTest2[6] = {'T','e','s','t','2','\n'};

int AppendingAndReading(int argc, char* argv[])
{
    // checks if i can create a file, and then both read and write to it at the same
    // time with different classes. The test aims only for the scenario of reading from the part
    // that is not written in, as would be the case for PDFs with incremental updates
    
    int status = 0;
    
    do 
    {
        // write initial file
        {
            OutputFile outputFile;
            
            if(outputFile.OpenFile(BuildRelativeOutputPath(argv, "ReadAndWrite.txt")) != eSuccess)
            {
                cout<<"Failed to open file for writing"<<endl;
                status = 1;
                break;
            }
            
            
            outputFile.GetOutputStream()->Write(scTest,5);
            outputFile.GetOutputStream()->Write(scTest1,6);
            outputFile.GetOutputStream()->Write(scTest2,6);
        }
    
        // now open file for read and file for append
        
        InputFile inputFile;
        
        if(inputFile.OpenFile(BuildRelativeOutputPath(argv, "ReadAndWrite.txt")) != eSuccess)
        {
            cout<<"Failed to open file for reading"<<endl;
            status = 1;
            break;
            
        }
        
        OutputFile appendToFile;
        
        if(appendToFile.OpenFile(BuildRelativeOutputPath(argv,"ReadAndWrite.txt"),true) != eSuccess)
        {
            cout<<"Failed to open file for appending"<<endl;
            status = 1;
            break;
            
        }
        
        // check reading 1
        inputFile.GetInputStream()->SetPosition(5);
        Byte bufferTest1[6];
        
        inputFile.GetInputStream()->Read(bufferTest1,6);
        if(memcmp(bufferTest1,scTest1,6) != 0)
        {
            cout<<"failed comparison in reading 1"<<endl;
            status = 1;
            break;           
        }
        
        
        // check writing 1
        appendToFile.GetOutputStream()->Write(scTest1,6);
        
        // check reading 2
        inputFile.GetInputStream()->Read(bufferTest1,6);
        if(memcmp(bufferTest1,scTest2,6) != 0)
        {
            cout<<"failed comparison in reading 2"<<endl;
            status = 1;
            break;           
        }
        
        // check reading 3
        inputFile.GetInputStream()->SetPosition(0);
        Byte bufferTest[5];
        inputFile.GetInputStream()->Read(bufferTest,5);
        if(memcmp(bufferTest,scTest,5) != 0)
        {
            cout<<"failed comparison in reading 3"<<endl;
            status = 1;
            break;           
        }
      
        // check writing 2
        appendToFile.GetOutputStream()->Write(scTest1,6);
        
    } 
    while(false);
 
    return status;
}

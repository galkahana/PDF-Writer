/*
   Source File : IReadInterrupt.h


   Copyright 2026 Gal Kahana PDFWriter

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

/*
	IReadInterrupt lets the application stop a long stream read (PDFParsingOptions::ReadInterrupt).
	A capped stream reader asks it about once per MiB it reads; true ends the stream there, and
	PDFParser::GetInterruptedStreamsCount counts it.
*/
class IReadInterrupt
{
public:
	virtual ~IReadInterrupt(){}

	virtual bool ShouldStop() = 0;
};

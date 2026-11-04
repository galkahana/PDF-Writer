/*
Source File : PDFParsingOptions.h


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

#include "IOBasicTypes.h"

#include <string>

class IReadInterrupt;

struct PDFParsingOptions
{
	std::string Password;

	// the most bytes a stream reader made by PDFParser::CreateInputStreamReader gives, for any stream, filtered
	// (the bytes its filters decode to) or not. the stream ends there, and PDFParser::GetTruncatedStreamsCount
	// counts it. 0, the default, sets no limit
	IOBasicTypes::LongBufferSizeType MaxStreamReadSize;
	// asked about once per MiB those readers give whether to stop reading (see IReadInterrupt.h). true ends the
	// stream, and PDFParser::GetInterruptedStreamsCount counts it. not owned, and must outlive the parser's readers.
	// NULL, the default, never stops a read
	IReadInterrupt* ReadInterrupt;

	PDFParsingOptions() { MaxStreamReadSize = 0; ReadInterrupt = NULL; }
	PDFParsingOptions(std::string inPassword) { Password = inPassword; MaxStreamReadSize = 0; ReadInterrupt = NULL; }

	static const PDFParsingOptions& DefaultPDFParsingOptions();
};
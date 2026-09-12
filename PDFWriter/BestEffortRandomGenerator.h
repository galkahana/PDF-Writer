/*
   Source File : BestEffortRandomGenerator.h


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

#include "IOBasicTypes.h"
#include <stddef.h>

// Best-effort random bytes for call sites that cannot rely on OpenSSL.
// Prefers the platform CSPRNG (BCryptGenRandom on Windows when available, arc4random_buf on
// Apple/BSD, /dev/urandom elsewhere on POSIX) and falls back to a
// seeded rand() only when none of those are available.
class BestEffortRandomGenerator
{
public:
	static void FillBytes(IOBasicTypes::Byte* outBuffer, size_t inSize);
};

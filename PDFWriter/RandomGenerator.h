/*
   Source File : RandomGenerator.h


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

#include "EStatusCode.h"
#include "IOBasicTypes.h"
#include <stddef.h>

// Random bytes for cryptographic use (AES IVs, PDF 2.0 key material). Uses RAND_bytes when
// OpenSSL is available, otherwise the platform CSPRNG (BCryptGenRandom on Windows,
// arc4random_buf on Apple/BSD, /dev/urandom elsewhere on POSIX).
class RandomGenerator
{
public:
	static PDFHummus::EStatusCode FillBytes(IOBasicTypes::Byte* outBuffer, size_t inSize);

#ifndef PDFHUMMUS_NO_OPENSSL
	// test-only hook to force RAND_bytes failures. pass NULL to restore the real RAND_bytes.
	static void SetRandBytesFunc(int (*inRandBytesFunc)(unsigned char*, int));
#endif
};

/*
   Source File : OutputAESEncodeStreamTest.cpp


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


   Targets whichever AES output stream the build actually links -
   OutputAESEncodeStreamSSL (OpenSSL-backed) when USE_OPENSSL_AES is on,
   else OutputAESEncodeStream (libaesgm-backed) - mirroring the same
   dispatch EncryptionHelper::CreateEncryptionWriter uses, paired with
   the matching Input*DecodeStream for round-trip decoding.

*/

#ifdef USE_OPENSSL_AES
#include "OutputAESEncodeStreamSSL.h"
#include "InputAESDecodeStreamSSL.h"
typedef OutputAESEncodeStreamSSL AesEncodeStream;
typedef InputAESDecodeStreamSSL AesDecodeStream;
#else
#include "OutputAESEncodeStream.h"
#include "InputAESDecodeStream.h"
typedef OutputAESEncodeStream AesEncodeStream;
typedef InputAESDecodeStream AesDecodeStream;
#endif

#include "OutputStringBufferStream.h"
#include "InputByteArrayStream.h"
#include "ByteList.h"

#include <iostream>
#include <string>

using namespace std;
using namespace IOBasicTypes;

static const size_t kIVSize = 16;

static ByteList MakeTestKey()
{
    ByteList key;
    for (int i = 0; i < 16; ++i)
        key.push_back((Byte)i);
    return key;
}

static string EncryptToBuffer(const string& inPlaintext, const ByteList& inKey)
{
    OutputStringBufferStream outputSink;
    AesEncodeStream* encodeStream = new AesEncodeStream(&outputSink, inKey, false);
    encodeStream->Write((const Byte*)inPlaintext.c_str(), inPlaintext.size());
    delete encodeStream; // flushes the final padded block
    return outputSink.ToString();
}

static string DecryptBuffer(const string& inEncrypted, const ByteList& inKey)
{
    IByteReader* source = new InputByteArrayStream((Byte*)&inEncrypted[0], (LongFilePositionType)inEncrypted.size());
    AesDecodeStream decodeStream(source, inKey);

    string recovered;
    Byte buffer[256];
    while (decodeStream.NotEnded())
    {
        LongBufferSizeType readSize = decodeStream.Read(buffer, sizeof(buffer));
        if (readSize == 0)
            break;
        recovered.append((const char*)buffer, readSize);
    }
    return recovered;
}

static bool Write_MultipleEncryptionsSameKey_ProducesDistinctIVs()
{
    // Arrange
    ByteList key = MakeTestKey();
    string plaintext = "the quick brown fox jumps over the lazy dog";

    // Act
    string firstOutput = EncryptToBuffer(plaintext, key);
    string secondOutput = EncryptToBuffer(plaintext, key);

    // Assert
    if (firstOutput.size() < kIVSize || secondOutput.size() < kIVSize)
    {
        cout << "OutputAESEncodeStreamTest [Write_MultipleEncryptionsSameKey_ProducesDistinctIVs]: output too short to contain an IV" << endl;
        return false;
    }
    if (firstOutput.substr(0, kIVSize) == secondOutput.substr(0, kIVSize))
    {
        cout << "OutputAESEncodeStreamTest [Write_MultipleEncryptionsSameKey_ProducesDistinctIVs]: IVs are identical across successive encryptions" << endl;
        return false;
    }
    return true;
}

static bool Write_EncryptThenDecrypt_RecoversOriginalPlaintext()
{
    // Arrange
    ByteList key = MakeTestKey();
    string plaintext = "round trip regression coverage for AES-CBC encode/decode";

    // Act
    string encrypted = EncryptToBuffer(plaintext, key);
    string recovered = DecryptBuffer(encrypted, key);

    // Assert
    if (recovered != plaintext)
    {
        cout << "OutputAESEncodeStreamTest [Write_EncryptThenDecrypt_RecoversOriginalPlaintext]: recovered plaintext does not match original" << endl;
        return false;
    }
    return true;
}

static bool Flush_NoWriteCalls_EncryptsAndRecoversEmptyPlaintext()
{
    // Arrange
    ByteList key = MakeTestKey();
    OutputStringBufferStream outputSink;
    AesEncodeStream* encodeStream = new AesEncodeStream(&outputSink, key, false);

    // Act
    delete encodeStream; // no Write() calls at all - Flush() must self-initialize the IV/cipher
    string encrypted = outputSink.ToString();

    // Assert
    if (encrypted.size() != kIVSize + 16) // IV + one padded block
    {
        cout << "OutputAESEncodeStreamTest [Flush_NoWriteCalls_EncryptsAndRecoversEmptyPlaintext]: expected IV plus one padded block, got " << encrypted.size() << " bytes" << endl;
        return false;
    }
    if (DecryptBuffer(encrypted, key) != "")
    {
        cout << "OutputAESEncodeStreamTest [Flush_NoWriteCalls_EncryptsAndRecoversEmptyPlaintext]: recovered plaintext is not empty" << endl;
        return false;
    }
    return true;
}

int OutputAESEncodeStreamTest(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    if (!Write_MultipleEncryptionsSameKey_ProducesDistinctIVs()) return 1;
    if (!Write_EncryptThenDecrypt_RecoversOriginalPlaintext()) return 1;
    if (!Flush_NoWriteCalls_EncryptsAndRecoversEmptyPlaintext()) return 1;
    return 0;
}

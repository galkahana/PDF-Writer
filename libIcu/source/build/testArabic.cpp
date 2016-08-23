#include "unicode/ptypes.h"
#include "unicode/urename.h"
#include "unicode/unistr.h"
#include "unicode/ushape.h"
#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <string>

using std::cout;
using std::endl;
//
// void
// printUString(const char *announce, const UChar *s, int32_t length) {
//     UConverter *cnv=NULL;
//     static char out[200];
//     UChar32 c;
//     int32_t i;
//     UErrorCode errorCode=U_ZERO_ERROR;
//
//     /*
//      * Convert to the "platform encoding". See notes in printUnicodeString().
//      * ucnv_fromUChars(), like most ICU APIs understands length==-1
//      * to mean that the string is NUL-terminated.
//      */
//     ucnv_fromUChars(cnv, out, sizeof(out), s, length, &errorCode);
//     if(U_FAILURE(errorCode) || errorCode==U_STRING_NOT_TERMINATED_WARNING) {
//         printf("%sproblem converting string from Unicode: %s\n", announce, u_errorName(errorCode));
//         return;
//     }
//
//     printf("%s%s {", announce, out);
//
//     /* output the code points (not code units) */
//     if(length>=0) {
//         /* s is not NUL-terminated */
//         for(i=0; i<length; /* U16_NEXT post-increments */) {
//             U16_NEXT(s, i, length, c);
//             printf(" %04x", c);
//         }
//     } else {
//         /* s is NUL-terminated */
//         for(i=0; /* condition in loop body */; /* U16_NEXT post-increments */) {
//             U16_NEXT(s, i, length, c);
//             if(c==0) {
//                 break;
//             }
//             printf(" %04x", c);
//         }
//     }
//     printf(" }\n");
// }


int main() {
  UErrorCode errorCode = U_ZERO_ERROR;
  static const UChar
      source[]={
          0x0020,0x0020,0x0020,0xFE8D,0xFEF5,0x0020,0xFEE5,0x0020,0xFE8D,0xFEF7,0x0020,
          0xFED7,0xFEFC,0x0020,0xFEE1,0x0020,0xFE8D,0xFEDF,0xFECC,0xFEAE,0xFE91,0xFEF4,
          0xFE94,0x0020,0xFE8D,0xFEDF,0xFEA4,0xFEAE,0xFE93,0x0020,0x0020,0x0020,0x0020
      };
  std::string s = u8"عمرو Shahin";
  UnicodeString src_text(source);//= UnicodeString::fromUTF8(s.c_str());
  UChar shaped_text[1000];
  char visString[1000];
  src_text.extract(visString, 1000, NULL, errorCode);
  cout<<endl<<"* sample_arabic_shaping() ---------- ***\n\n";
  // printUString("Source text", src_text, src_text.length());
  cout<<"Source: "<<visString<<endl;
  std::cout<<std::endl;
  // U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_NEAR|
  //                          U_SHAPE_TEXT_DIRECTION_VISUAL_LTR
  u_shapeArabic   (   src_text.getBuffer(),
  src_text.length(),
  shaped_text,
  1000,
  U_SHAPE_LETTERS_UNSHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_BEGINNING|
U_SHAPE_TEXT_DIRECTION_LOGICAL,

   &errorCode
  );

  cout<<"Error: " << u_errorName(errorCode) << endl;
  UnicodeString dest_text(shaped_text);
  std::string utf8;
  dest_text.toUTF8String(utf8);


  cout<< "Shaped Text: "<< utf8<< endl;
  return 0;
}

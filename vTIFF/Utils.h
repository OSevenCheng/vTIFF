#pragma once
//typedef byte uchar;

#include <Map>
#define byte unsigned char
class OColor
{
public:
    OColor(float _r,float _g,float _b,float _a):r(_r),g(_g),b(_b),a(_a){}
    float r;
    float g;
    float b;
    float a;
};
const int TypeArray[] = {//"???",
            0,
            1,//byte //8-bit unsigned integer
                   1,//ascii//8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL (binary zero)
                   2, //short",2),//16-bit (2-byte) unsigned integer.
                  4,//  long",4),//32-bit (4-byte) unsigned integer.
                  8,//  rational",8),//Two LONGs: the first represents the numerator of a fraction; the second, the denominator.
                  1,//  sbyte",1),//An 8-bit signed (twos-complement) integer
                 1,//   undefined",1),//An 8-bit byte that may contain anything, depending on the definition of the field
                 2,//   sshort",1),//A 16-bit (2-byte) signed (twos-complement) integer.
                 4,//   slong",1),// A 32-bit (4-byte) signed (twos-complement) integer.
                8,//    srational",1),//Two SLONG¡¯s: the first represents the numerator of a fraction, the second the denominator.
                4,//    float",4),//Single precision (4-byte) IEEE format
                8//    double",8)//Double precision (8-byte) IEEE format
};
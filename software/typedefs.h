#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef unsigned char   byte;           // 8-bit
typedef unsigned int    word;           // 16-bit
typedef unsigned long   dword;          // 32-bit

typedef union _BYTE
{
    byte _byte;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    };
} BYTE1;

typedef union _WORD
{
    word _word;
    struct
    {
        byte byte0;
        byte byte1;
    };
    struct
    {
        BYTE1 Byte0;
        BYTE1 Byte1;
    };
    struct
    {
        BYTE1 LowB;
        BYTE1 HighB;
    };
    struct
    {
        byte v[2];
    };
} WORD1;
#define LSB(a)      ((a).v[0])
#define MSB(a)      ((a).v[1])

typedef union _DWORD
{
    dword _dword;
    struct
    {
        byte byte0;
        byte byte1;
        byte byte2;
        byte byte3;
    };
    struct
    {
        word word0;
        word word1;
    };
    struct
    {
        BYTE1 Byte0;
        BYTE1 Byte1;
        BYTE1 Byte2;
        BYTE1 Byte3;
    };
    struct
    {
        WORD1 Word0;
        WORD1 Word1;
    };
    struct
    {
        byte v[4];
    };
} DWORD1;
#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])


#define OK      TRUE
#define FAIL    FALSE

#endif //TYPEDEFS_H

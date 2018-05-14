/*
 *-------------------------------------------------------------------------
 *
 * CCITT G.711 u-law, A-law and linear PCM conversions.
 *       Version 94/12/30 - COST 232
 *
 * Usage :
 *
 * COSTG711 -au|ua|la|al|lu|ul|ll infile outfile
 *
 *     -au: A-law to u-law conversion
 *     -ua: u-law to A-law conversion
 *     -la: 16-bit linear PCM to A-law
 *     -al: A-law to 16-bit linear PCM
 *     -lu: 16-bit linear PCM to u-law
 *     -ul: u-law to 16-bit linear PCM
 *     -ll: 16-bit linear low/high byte swapping
 *
 *-------------------------------------------------------------------------
 *
 * The source code of Sun Microsystems, referenced below, has been modified by
 *
 *    Boerge Lindberg
 *    Center for PersonKommunikation
 *    Aalborg University, Denmark
 *    e-mail : bli@cpk.auc.dk
 *
 * on behalf of the now concluded COST 232 Project on
 *
 *    "Speech Recognition over the Telephone Network".
 *
 * Modifications concern the addition of a main program for file processing.
 * Additonal changes:
 *
 * 94/02/10 : Low/high byte swapping added.
 * 94/12/30 : Bugs fixed:
 *
 *            Functions linear2alaw, linear2ulaw have been updated to correctly
 *            convert unquantized 16 bit values.
 *            Tables for direct u- to A-law and A- to u-law conversions have been
 *            corrected.
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "endecode.h"

static short seg_aend[8] = {
    0x1F, 0x3F, 0x7F, 0xFF,
    0x1FF, 0x3FF, 0x7FF, 0xFFF};
/* copy from CCITT G.711 specifications */
unsigned char _u2a[128] = {/* u- to A-law conversions */
                           1,  1,  2,  2,  3,  3,  4,  4,
                           5,  5,  6,  6,  7,  7,  8,  8,
                           9,  10, 11, 12, 13, 14, 15, 16,
                           17, 18, 19, 20, 21, 22, 23, 24,
                           25, 27, 29, 31, 33, 34, 35, 36,
                           37, 38, 39, 40, 41, 42, 43, 44,
                           46, 48, 49, 50, 51, 52, 53, 54,
                           55, 56, 57, 58, 59, 60, 61, 62,
                           64, 65, 66, 67, 68, 69, 70, 71,
                           72, 73, 74, 75, 76, 77, 78, 79,
//                         corrected:
//                           81,    82,    83,    84,    85,    86,    87,    88,
//                         should be:
                           80,     82,     83,     84,     85,     86,     87,     88,
                           89,     90,     91,     92,     93,     94,     95,     96,
                           97,     98,     99,     100,    101,    102,    103,    104,
                           105,    106,    107,    108,    109,    110,    111,    112,
                           113,    114,    115,    116,    117,    118,    119,    120,
                           121,    122,    123,    124,    125,    126,    127,    128};

unsigned char _a2u[128] = {/* A- to u-law conversions */
                           1,  3,  5,  7,  9,  11, 13, 15,
                           16, 17, 18, 19, 20, 21, 22, 23,
                           24, 25, 26, 27, 28, 29, 30, 31,
                           32, 32, 33, 33, 34, 34, 35, 35,
                           36, 37, 38, 39, 40, 41, 42, 43,
                           44, 45, 46, 47, 48, 48, 49, 49,
                           50, 51, 52, 53, 54, 55, 56, 57,
                           58, 59, 60, 61, 62, 63, 64, 64,
                           65, 66, 67, 68, 69, 70, 71, 72,
//                         corrected:
//                         73,    74,    75,    76,    77,    78,    79,    79,
//                         should be:
                           73, 74, 75, 76, 77, 78, 79, 80,

                           80,  81,  82,   83,  84,  85,  86,  87,
                           88,  89,  90,   91,  92,  93,  94,  95,
                           96,  97,  98,   99,  100, 101, 102, 103,
                           104, 105, 106, 107, 108, 109, 110, 111,
                           112, 113, 114, 115, 116, 117, 118, 119,
                           120, 121, 122, 123, 124, 125, 126, 127};


/* 16 bit swapping */
short EDcode::swap_linear (short pcm_val)
{
    struct lohibyte { unsigned char lb, hb;};
    union { struct lohibyte b;
            short i;
          } exchange;
    unsigned char c;

    exchange.i      = pcm_val;
    c               = exchange.b.hb;
    exchange.b.hb   = exchange.b.lb;
    exchange.b.lb   = c;
    return (exchange.i);
}

//16 bits PCM to alaw
void EDcode::encode( EDcode::shortbuffer inbuf,unsigned char *compbuf)
{
    short i;
    unsigned short count=BLOCK_SIZE/2;

    for (i=0; i < count; i++)
        compbuf[i] = EDcode::linear2alaw(inbuf.samples_short[i]);
}

//alaw to PCM 16 bits
void EDcode::decode(unsigned char inbuf[], short *outbuf)
{
    short i;
    unsigned short      in_size, out_size, count;

    count = 64;
    in_size = sizeof (char);
    out_size = sizeof (short);

    for (i=0; i < count; i++)
        outbuf[i] = EDcode::alaw2linear(inbuf[i]);
}

static short search(short val, short *table, short size)
{
    short   i;

    for (i = 0; i < size; i++) {
        if (val <= *table++)
            return (i);
    }
    return (size);
}

/*
 * linear2alaw() - Convert a 16-bit linear PCM value to 8-bit A-law
 *
 * linear2alaw() accepts an 16-bit integer and encodes it as A-law data.
 *
 *        Linear Input Code    Compressed Code
 *    ------------------------    ---------------
 *    0000000wxyza            000wxyz
 *    0000001wxyza            001wxyz
 *    000001wxyzab            010wxyz
 *    00001wxyzabc            011wxyz
 *    0001wxyzabcd            100wxyz
 *    001wxyzabcde            101wxyz
 *    01wxyzabcdef            110wxyz
 *    1wxyzabcdefg            111wxyz
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */
unsigned char EDcode::linear2alaw( short pcm_val)    /* 2's complement (16-bit range) */
{
    short        mask;
    short        seg;
    unsigned char    aval;

    pcm_val = pcm_val >> 3;

    if (pcm_val >= 0) {
        mask = 0xD5;        /* sign (7th) bit = 1 */
    } else {
        mask = 0x55;        /* sign bit = 0 */
        pcm_val = -pcm_val - 1;
    }

    /* Convert the scaled magnitude to segment number. */
    seg = search(pcm_val, seg_aend, 8);

    /* Combine the sign, segment, and quantization bits. */

    if (seg >= 8)        /* out of range, return maximum value. */
        return (unsigned char) (0x7F ^ mask);
    else {
        aval = (unsigned char) seg << SEG_SHIFT;
        if (seg < 2)
            aval |= (pcm_val >> 1) & QUANT_MASK;
        else
            aval |= (pcm_val >> seg) & QUANT_MASK;
        return (aval ^ mask);
    }
}

/*
 * alaw2linear() - Convert an A-law value to 16-bit linear PCM
 *
 */
short EDcode::alaw2linear( unsigned char a_val)
{
    short    t;
    short    seg;

    a_val ^= 0x55;

    t = (a_val & QUANT_MASK) << 4;
    seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
    switch (seg) {
    case 0:
        t += 8;
        break;
    case 1:
        t += 0x108;
        break;
    default:
        t += 0x108;
        t <<= seg - 1;
    }
    return ((a_val & SIGN_BIT) ? t : -t);
}

//#define BIAS    (0x84)        /* Bias for linear code. */
//#define CLIP    8159

/*
 * linear2ulaw() - Convert a linear PCM value to u-law
 *
 * In order to simplify the encoding process, the original linear magnitude
 * is biased by adding 33 which shifts the encoding range from (0 - 8158) to
 * (33 - 8191). The result can be seen in the following encoding table:
 *
 *    Biased Linear Input Code    Compressed Code
 *    ------------------------    ---------------
 *    00000001wxyza            000wxyz
 *    0000001wxyzab            001wxyz
 *    000001wxyzabc            010wxyz
 *    00001wxyzabcd            011wxyz
 *    0001wxyzabcde            100wxyz
 *    001wxyzabcdef            101wxyz
 *    01wxyzabcdefg            110wxyz
 *    1wxyzabcdefgh            111wxyz
 *
 * Each biased linear code has a leading 1 which identifies the segment
 * number. The value of the segment number is equal to 7 minus the number
 * of leading 0's. The quantization interval is directly available as the
 * four bits wxyz.  * The trailing bits (a - h) are ignored.
 *
 * Ordinarily the complement of the resulting code word is used for
 * transmission, and so the code word is complemented before it is returned.
 *
 * For further information see John C. Bellamy's Digital Telephony, 1982,
 * John Wiley & Sons, pps 98-111 and 472-476.
 */
//unsigned char linear2ulaw( short pcm_val)    /* 2's complement (16-bit range) */
//{
//    short        mask;
//    short        seg;
//    unsigned char    uval;

//    /* Get the sign and the magnitude of the value. */
//    pcm_val = pcm_val >> 2;
//    if (pcm_val < 0) {
//        pcm_val = -pcm_val;
//        mask = 0x7F;
//    } else {
//        mask = 0xFF;
//    }
//    if ( pcm_val > CLIP )
//        pcm_val = CLIP;        /* clip the magnitude */

//    pcm_val += (BIAS >> 2);

//    /* Convert the scaled magnitude to segment number. */
//    seg = search(pcm_val, seg_uend, 8);

//    /*
//     * Combine the sign, segment, quantization bits;
//     * and complement the code word.
//     */
//    if (seg >= 8)        /* out of range, return maximum value. */
//        return (unsigned char) (0x7F ^ mask);
//    else {
//        uval = (unsigned char) (seg << 4) | ((pcm_val >> (seg + 1)) & 0xF);
//        return (uval ^ mask);
//    }

//}

///*
// * ulaw2linear() - Convert a u-law value to 16-bit linear PCM
// *
// * First, a biased linear code is derived from the code word. An unbiased
// * output can then be obtained by subtracting 33 from the biased code.
// *
// * Note that this function expects to be passed the complement of the
// * original code word. This is in keeping with ISDN conventions.
// */
//short ulaw2linear(unsigned char u_val)
//{
//    short t;

//    /* Complement to obtain normal u-law value. */
//    u_val = ~u_val;
//    /*
//     * Extract and bias the quantization bits. Then
//     * shift up by the segment number and subtract out the bias.
//     */
//    t = ((u_val & QUANT_MASK) << 3) + BIAS;
//    t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

//    return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
//}

///* A-law to u-law conversion */
//unsigned char alaw2ulaw(unsigned char aval)
//{
//    aval &= 0xff;
//    return (unsigned char) ((aval & 0x80) ? (0xFF ^ _a2u[aval ^ 0xD5]) :
//        (0x7F ^ _a2u[aval ^ 0x55]));
//}

///* u-law to A-law conversion */
//unsigned char ulaw2alaw(unsigned char uval)
//{
//    uval &= 0xff;
//    return (unsigned char) ((uval & 0x80) ? (0xD5 ^ (_u2a[0xFF ^ uval] - 1)) :
//        (unsigned char) (0x55 ^ (_u2a[0x7F ^ uval] - 1)));
//}





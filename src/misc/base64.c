/*
 * Base64 Encoder, base64.c
 * Copyright (C) 2001 ***REMOVED***
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "pircd.h"
#include "h.h"

#include <assert.h>

__RCSID("$Id: base64.c,v 1.1 2001/09/27 00:41:18 mysidia Exp $");

/*************************************************************************/

/**
 * Encode in base64 - finally got this working, I hope. -Mysid
 *
 * \param stream Memory area to read character data from
 * \param left   Size of the memory area in bytes
 * \pre Stream points to a valid (data-octet-bearing) character array of
 *      size 'left' or higher. 
 * \return A base64-encoded version of binary data read from `stream' up
 *         to `left' octets.  The version returned will have been
 *         allocated from the heap.  Freeing it is the caller's
 *         responsibility.
 */
unsigned char *toBase64(const unsigned char *stream, size_t left)
{
	#define X_OUTPUT_MAP_SZ 64
	const unsigned char mutations[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	unsigned int j, k, s_sz = 0, acc;
	unsigned char temp[3], out[4];
	char *s = NULL, *p;

	j = acc = 0;

	// Calculate the length increase from the conversion from
	// 8 bit to 6 bit encoding. We will even provide a null terminator.

	s_sz = (((left * 8) / 24) * 4) +
		((((left * 8) % 24)) ? 3 : 0) +
		2;


	p = s = (char *)oalloc(s_sz + 1);

	for ( ; left ; )
	{
		while ( left > 0 && j < 3 ) {
			temp[j++] = *stream;
			left--;
			stream++;
		}

		//
		// Alphabetic approximation of what our
		// bitwise operations need to accomplish
		//
		// ABCDEFGH IJKLMNOP QRSTUVWX --------
		// ABCDEFGH IJKLMNOP QRSTUVWX STUVWX--
		// --ABCDEF ABCDEFGH --MNOP-- STUVWX--
		// 00111111 00000011 ------QR STUVWX--
		// --ABCDEF ------GH --MNOPQR STUVWX--
		// --ABCDEF ----GH-- --MNOPQR STUVWX--
		// --ABCDEF ----IJKL --MNOPQR STUVWX--
		// --ABCDEF --GHIJKL --MNOPQR STUVWX--
		// --ABCDEF --GHIJKL --MNOPQR STUVWX--
		//

		// Stuff the first 6 bits into the 1st 6-bit counter
		// Shift it over so that the 2 high bits are
		// zero
		out[0] = (temp[0] >> 2);

		// Stuff the last two bits of the first byte into
		// the second counter -- shift it into the high
		// bits.  Carry in the next 4 bits
		{
			if (j < 3) {
				temp[2] = 0;
				if (j < 2)
					temp[1] = 0;
			}

			out[1] = ((temp[0] & 0x03) << 4) |
			         (temp[1] >> 4);

			// Lather, rinse, repeat

			// 00001111 -> 0x0f
			out[2] = ( (temp[1] & 0x0f) << 2 ) |
	 		         ( temp[2] >> 6 );

			// 00111111 ->  0x3f
			out[3] = (temp[2] & 0x3f);
		}

		for(k = 0; k < 4; k++) {
			if ( k <= j ) {
				assert(out[k] < X_OUTPUT_MAP_SZ);
				*p++ = mutations[out[k]];
			}
			else	*p++ = '=';
			if (--s_sz < 1)
				assert(s_sz >= 1);
		}

		j = 0;
	}

	*p++ = '\0';

	#undef X_OUTPUT_MAP_SZ 64
	return (unsigned char *)s;
}

/**
 * Decode from base64 - whole lot easier to decode than to encode <G>
 *
 * \param cStr Memory area to read character data from
 * \pre Stream points to a valid NUL-terminated character array of the
 *      ASCII character set that is a valid base-64 encoding of binary
 *      data
 * \return A number of octets binary data as decoded from the input.
 *         Allocated from the heap, caller must free.
 */
unsigned char *fromBase64(const char *cStr, int *len)
{
	#define X_OUTPUT_MAP_SZ 64
	const unsigned char mutations[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	static unsigned char decode_table[255] = {};
	static int pFlag = 0;
	int i, j, k, s_sz = 0;
	unsigned char temp[4], out[4], c;
	unsigned char *s = NULL, *p;
	size_t left = strlen(cStr);


	if (pFlag == 0)
	{
		pFlag = 1;

		for(j = 0; j < 127; j++)
			decode_table[j] = '\0';

		for(j = 0; j < 64; j++) {
			for(i = 0; i < 127; i++) {
				if (i == mutations[j]) {
					decode_table[i] = j;
					break;
				}
			}
		}
		pFlag = 1;
	}

	// For every 4 characters we have, we have 24 bits of output.
        // 24 / 8 ==> 3 bytes

	s_sz = 2 + (int) ((left / 4.0) * 3);

	p = s = (unsigned char *)oalloc(s_sz + 1);

	for ( ; left > 0 ; )
	{
		j = 0;

		while(j < 4 && left > 0) {
			c = *(cStr++);
			if ((unsigned char)temp[j] < sizeof(decode_table))
				c = decode_table[(unsigned char)c];
			else c = '\0';

			if (c != '\0') {
				temp[j] = c;
				j++;
			}
			left--;
		}
		for(k = j; k < 4; k++)
			temp[k] = 0;

		// --ABCDEF --GHIJKL --MNOPQR STUVWX--
		// ABCDEFGH IJKL---- QR------
		// ------GH ----MNOP --STUVWX

		out[0] = (temp[0] << 2) | ((temp[1] >> 4) & 0x03);
		out[1] = (temp[1] << 4) | ((temp[2] >> 2));
		out[2] = (temp[2] << 6) | temp[3];

		if (s_sz > 0) {
			*(p++) = out[0];
			s_sz--;
		}

		if (s_sz > 0) {
			*(p++) = out[1];
			s_sz--;
		}

		if (j >= 4) {
			(*(p++)) = out[2];
			s_sz--;
		}

		if (s_sz <= 0) {
			assert(s_sz > 0);
		}
	}

	if (len)
		(*len) = (size_t)(p - s);

	*p++ = '\0';

	return (unsigned char *)s;
	#undef X_OUTPUT_MAP_SZ
}

/*
 * -> Base on 100million in the most gruesome way imaginable and map it
 * into 65 characters, in a peculiar way, the heck? *snick* 
 *
 * -Mysid 
 */

char *B64_MuckIntValUp(u_int32_t value)
{
   static char result[256];
   char *x_output_map[] = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "-", "_", /*"0",*/ "1", "2", "3", "4", "5", "6", "7", "8", "9", "/",
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "$", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",

        "*", "+", "=", "\\", "&", "^", "#", "@", "!", "{",
        "(", "[" "?", "'", "\"", "]", ")", ")", ",", ".",
        "<", ">", "%", "`", /*after this point not used*/"|", "`", ""
   };
   #define X_OUTPUT_MAP_SZ 88
   #define X_BASE_ENCODING ULONG_MAX
   int i = 0, x = 99;
   u_int32_t rem;

   for(i = 0; i < 100; i++)
       result[i] = '\0';

   while(value > 0) {
        rem = (value % X_BASE_ENCODING);
        value = (int) value / X_BASE_ENCODING;
        if ((rem >= X_BASE_ENCODING))
            break;
        result[x--] = *x_output_map[((unsigned char)rem) % X_OUTPUT_MAP_SZ];
        if (x < 0 || value < 1)
            break;
   }

   if (x < 99)
       for(i = 0; !result[i]; i++) ;
   else i = 0;

   return (result + i);
   #undef X_OUTPUT_MAP_SZ
   #undef X_BASE_ENCODING
}

/*************************************************************************/


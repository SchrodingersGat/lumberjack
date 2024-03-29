// fieldencode.hpp was generated by ProtoGen version 3.2.a

#ifndef _FIELDENCODE_HPP
#define _FIELDENCODE_HPP

// Language target is C++

/*!
 * \file
 * fieldencode provides routines to place numbers into a byte stream.
 * 
 * fieldencode provides routines to place numbers in local memory layout into a
 * big or little endian byte stream. The byte stream is simply a sequence of
 * bytes, as might come from the data payload of a packet.
 * 
 * Support is included for non-standard types such as unsigned 24. When working
 * with nonstandard types the data in memory are given using the next larger
 * standard type. For example an unsigned 24 is actually a uint32_t in which
 * the most significant byte is clear, and only the least significant three
 * bytes are placed into a byte stream
 * 
 * Big or Little Endian refers to the order that a computer architecture will
 * place the bytes of a multi-byte word into successive memory locations. For
 * example the 32-bit number 0x01020304 can be placed in successive memory
 * locations in Big Endian: [0x01][0x02][0x03][0x04]; or in Little Endian:
 * [0x04][0x03][0x02][0x01]. The names "Big Endian" and "Little Endian" come
 * from Swift's Gulliver's travels, referring to which end of an egg should be
 * opened. The choice of name is made to emphasize the degree to which the
 * choice of memory layout is un-interesting, as long as one stays within the
 * local memory.
 * 
 * When transmitting data from one computer to another that assumption no
 * longer holds. In computer-to-computer transmission there are three endians
 * to consider: the endianness of the sender, the receiver, and the protocol
 * between them. A protocol is Big Endian if it sends the most significant byte
 * first and the least significant last. If the computer and the protocol have
 * the same endianness then encoding data from memory into a byte stream is a
 * simple copy. However if the endianness is not the same then bytes must be
 * re-ordered for the data to be interpreted correctly.
 */


#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#include <stdbool.h>

//! Macro to limit a number to be no more than a maximum value
#define limitMax(number, max) (((number) > (max)) ? (max) : (number))

//! Macro to limit a number to be no less than a minimum value
#define limitMin(number, min) (((number) < (min)) ? (min) : (number))

//! Macro to limit a number to be no less than a minimum value and no more than a maximum value
#define limitBoth(number, min, max) (((number) > (max)) ? (max) : (limitMin((number), (min))))

//! Copy a null terminated string
void pgstrncpy(char* dst, const char* src, int maxLength);

//! Encode a null terminated string on a byte stream
void stringToBytes(const char* string, uint8_t* bytes, int* index, int maxLength, int fixedLength);

//! Copy an array of bytes to a byte stream without changing the order.
void bytesToBeBytes(const uint8_t* data, uint8_t* bytes, int* index, int num);

//! Copy an array of bytes to a byte stream while reversing the order.
void bytesToLeBytes(const uint8_t* data, uint8_t* bytes, int* index, int num);

//! Encode a 4 byte float on a big endian byte stream.
void float32ToBeBytes(float number, uint8_t* bytes, int* index);

//! Encode a 4 byte float on a little endian byte stream.
void float32ToLeBytes(float number, uint8_t* bytes, int* index);

//! Encode a unsigned 4 byte integer on a big endian byte stream.
void uint32ToBeBytes(uint32_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 4 byte integer on a little endian byte stream.
void uint32ToLeBytes(uint32_t number, uint8_t* bytes, int* index);

//! Encode a signed 4 byte integer on a big endian byte stream.
void int32ToBeBytes(int32_t number, uint8_t* bytes, int* index);

//! Encode a signed 4 byte integer on a little endian byte stream.
void int32ToLeBytes(int32_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 3 byte integer on a big endian byte stream.
void uint24ToBeBytes(uint32_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 3 byte integer on a little endian byte stream.
void uint24ToLeBytes(uint32_t number, uint8_t* bytes, int* index);

//! Encode a signed 3 byte integer on a big endian byte stream.
void int24ToBeBytes(int32_t number, uint8_t* bytes, int* index);

//! Encode a signed 3 byte integer on a little endian byte stream.
void int24ToLeBytes(int32_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 2 byte integer on a big endian byte stream.
void uint16ToBeBytes(uint16_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 2 byte integer on a little endian byte stream.
void uint16ToLeBytes(uint16_t number, uint8_t* bytes, int* index);

//! Encode a signed 2 byte integer on a big endian byte stream.
void int16ToBeBytes(int16_t number, uint8_t* bytes, int* index);

//! Encode a signed 2 byte integer on a little endian byte stream.
void int16ToLeBytes(int16_t number, uint8_t* bytes, int* index);

//! Encode a unsigned 1 byte integer on a byte stream.
#define uint8ToBytes(number, bytes, index) (bytes)[(*(index))++] = ((uint8_t)(number))

//! Encode a signed 1 byte integer on a byte stream.
#define int8ToBytes(number, bytes, index) (bytes)[(*(index))++] = ((int8_t)(number))

#endif // _FIELDENCODE_HPP

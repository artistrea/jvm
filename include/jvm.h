#ifndef JVM_H
#define JVM_H

// to consider: big endian when operator uses more than 1 byte
// e.g. 16bit var = ((1st byte) << 8) | (2nd byte)

// there are 2 types of values: reference and primitive values

// byte: -128 to 127
// short: -32768 to 32767
// int: -2147483648 to -2147483647
// long: -9223372036854775808 to -9223372036854775807
// char: '\u0000' to '\uffff', or, 0 to 65535

// no need for concurrency impl
// no need for garbage collector impl

#endif // JVM_H


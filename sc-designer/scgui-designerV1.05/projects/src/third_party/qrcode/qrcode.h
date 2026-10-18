/**
 * The MIT License (MIT)
 *
 * This library is written and maintained by Richard Moore.
 * Major parts were derived from Project Nayuki's library.
 *
 * Copyright (c) 2017 Richard Moore     (https://github.com/ricmoo/QRCode)
 * Copyright (c) 2017 Project Nayuki    (https://www.nayuki.io/page/qr-code-generator-library)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Special thanks to Nayuki (https://www.nayuki.io/) from which this library was
 *  heavily inspired and compared against.
 *
 *  See: https://github.com/nayuki/QR-Code-generator/tree/master/cpp
 */


#ifndef THIRD_PARTY_QRCODE_H
#define THIRD_PARTY_QRCODE_H

#include <stdbool.h>
#include <stdint.h>


#ifndef QRCODE_ENABLE
#ifdef SC_WIDGET_QCODE_ENABLE
#define QRCODE_ENABLE SC_WIDGET_QCODE_ENABLE
#elif defined(SC_USER_OBJ)
#define QRCODE_ENABLE SC_USER_OBJ
#else
#define QRCODE_ENABLE 1
#endif
#endif

/* Compile-time capacity: supported values are 17, 32, and 53 bytes. */
#ifndef SC_QCODE_MAX_BYTES
#define SC_QCODE_MAX_BYTES 53U
#endif

#if SC_QCODE_MAX_BYTES == 17U
#define QRCODE_VERSION 1U
#define QRCODE_SIZE 21U
#define QRCODE_RAW_DATA_MODULES 208U
#define QRCODE_ECC_CODEWORDS 7U
#elif SC_QCODE_MAX_BYTES == 32U
#define QRCODE_VERSION 2U
#define QRCODE_SIZE 25U
#define QRCODE_RAW_DATA_MODULES 359U
#define QRCODE_ECC_CODEWORDS 10U
#elif SC_QCODE_MAX_BYTES == 53U
#define QRCODE_VERSION 3U
#define QRCODE_SIZE 29U
#define QRCODE_RAW_DATA_MODULES 567U
#define QRCODE_ECC_CODEWORDS 15U
#else
#error SC_QCODE_MAX_BYTES must be 17, 32, or 53
#endif

#define QRCODE_TEXT_MAX SC_QCODE_MAX_BYTES
#define QRCODE_BUFFER_SIZE ((QRCODE_SIZE * QRCODE_SIZE + 7U) / 8U)
#define QRCODE_CODEWORD_BUFFER_SIZE ((QRCODE_RAW_DATA_MODULES + 7U) / 8U)


// QR Code Format Encoding
#define MODE_NUMERIC        0
#define MODE_ALPHANUMERIC   1
#define MODE_BYTE           2


// Error Correction Code Levels
#define ECC_LOW            0
#define ECC_MEDIUM         1
#define ECC_QUARTILE       2
#define ECC_HIGH           3


// If set to non-zero, this library can ONLY produce QR codes at that version
// This saves a lot of dynamic memory, as the codeword tables are skipped
#ifndef LOCK_VERSION
#define LOCK_VERSION       QRCODE_VERSION
#elif LOCK_VERSION != QRCODE_VERSION
#error LOCK_VERSION does not match SC_QCODE_MAX_BYTES
#endif

#ifndef QRCODE_BYTE_ONLY
#define QRCODE_BYTE_ONLY   1
#elif QRCODE_BYTE_ONLY != 1
#error This size-optimized port only supports byte mode
#endif


typedef struct QRCode {
    uint8_t version;
    uint8_t size;
    uint8_t ecc;
    uint8_t mode;
    uint8_t mask;
    uint8_t *modules;
} QRCode;


#ifdef __cplusplus
extern "C"{
#endif  /* __cplusplus */



#if QRCODE_ENABLE
int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, const char *data);
#endif



#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* THIRD_PARTY_QRCODE_H */

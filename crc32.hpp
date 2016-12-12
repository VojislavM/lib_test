

#ifndef KORUZA_CRC32_H
#define KORUZA_CRC32_H

#include <stdint.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Computes the CRC32 checksum over a buffer.
 *
 * @param crc Existing CRC32 value to use as a starting point
 * @param buf Input buffer
 * @param size Size of the input buffer
 * @return CRC32 checksum
 */
uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif

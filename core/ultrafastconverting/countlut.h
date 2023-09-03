#ifndef COUNTLUT_H
#define COUNTLUT_H
#include <stdint.h>

int u32toa_countlut(uint32_t value, char* buffer);
int i32toa_countlut(int32_t value, char* buffer);
int u64toa_countlut(uint64_t value, char* buffer);
int i64toa_countlut(int64_t value, char* buffer);

#endif // COUNTLUT_H

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// 使用 __attribute__((packed)) 确保 C++ 和内核驱动的内存布局完全对齐
struct __attribute__((packed)) CarSensorPacket {
    int32_t distance;      // 对应驱动 int
    int32_t temperature;   // 对应驱动 int
    int32_t humidity;      // 对应驱动 int
    uint64_t timestamp;    // 对应驱动 unsigned long (64位)
};

#endif
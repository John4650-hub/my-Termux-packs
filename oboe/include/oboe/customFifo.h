#ifndef CUSTOM_FIFO_BUFFER_H
#define CUSTOM_FIFO_BUFFER_H

#include <stdint.h>
#include "fifo/FifoBuffer.h"

namespace oboe {

class CustomFifoBuffer : public FifoBuffer {
public:
    CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames);

    int32_t read_custom(void* buffer, int32_t numFrames);

    void clearAfterRead(uint32_t readIndex, int32_t framesToRead);
};

} // namespace oboe

#endif // CUSTOM_FIFO_BUFFER_H

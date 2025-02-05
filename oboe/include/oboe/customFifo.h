#ifndef CUSTOM_FIFO_BUFFER_H
#define CUSTOM_FIFO_BUFFER_H

#include <stdint.h>
#include "oboe/FifoBuffer.h"

namespace oboe {

class CustomFifoBuffer : public FifoBuffer {
public:
    CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames);
    CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames,
                     int64_t* readIndexAddress, int64_t* writeIndexAddress,
                     uint8_t* dataStorageAddress);

    int32_t read(void* buffer, int32_t numFrames) override;

private:
    void clearAfterRead(uint32_t readIndex, int32_t framesToRead);
};

} // namespace oboe

#endif // CUSTOM_FIFO_BUFFER_H

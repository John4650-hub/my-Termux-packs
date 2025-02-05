#include <stdint.h>
#include <time.h>
#include <memory.h>
#include <assert.h>
#include "oboe/FifoBuffer.h"
#include "oboe/customFifo.h"
#include "oboe/FifoControllerBase.h"

namespace oboe {

CustomFifoBuffer::CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames)
    : FifoBuffer(bytesPerFrame, capacityInFrames) {}

CustomFifoBuffer::CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames,
                                   std::atomic<uint64_t>* readCounterAddress, std::atomic<uint64_t>* writeCounterAddress,
                                   uint8_t* dataStorageAddress)
    : FifoBuffer(bytesPerFrame, capacityInFrames, readCounterAddress, writeCounterAddress, dataStorageAddress) {}

int32_t CustomFifoBuffer::read(void* buffer, int32_t numFrames) {
    int32_t framesAvailable = getFullFramesAvailable();
    int32_t framesToRead = numFrames;
    if (framesToRead > framesAvailable) {
        framesToRead = framesAvailable;
    }
    if (framesToRead <= 0) {
        return 0;
    }

    uint32_t readIndex = getReadIndex();
    uint8_t* destination = reinterpret_cast<uint8_t*>(buffer);
    uint8_t* source = &mStorage[convertFramesToBytes(readIndex)];
    if ((readIndex + framesToRead) > mFrameCapacity) {
        uint32_t frames1 = mFrameCapacity - readIndex;
        int32_t numBytes = convertFramesToBytes(frames1);
        if (numBytes < 0) {
            return (int32_t) Result::ErrorOutOfRange;
        }
        memcpy(destination, source, (size_t) numBytes);
        destination += numBytes;
        source = &mStorage[0];
        int frames2 = framesToRead - frames1;
        numBytes = convertFramesToBytes(frames2);
        if (numBytes < 0) {
            return (int32_t) Result::ErrorOutOfRange;
        }
        memcpy(destination, source, (size_t) numBytes);
    } else {
        int32_t numBytes = convertFramesToBytes(framesToRead);
        if (numBytes < 0) {
            return (int32_t) Result::ErrorOutOfRange;
        }
        memcpy(destination, source, (size_t) numBytes);
    }
    clearAfterRead(readIndex, framesToRead);
    advanceReadIndex(framesToRead);

    return framesToRead;
}

void CustomFifoBuffer::clearAfterRead(uint32_t readIndex, int32_t framesToRead) {
    uint8_t* destination = &mStorage[convertFramesToBytes(readIndex)];
    if ((readIndex + framesToRead) > mFrameCapacity) {
        uint32_t frames1 = mFrameCapacity - readIndex;
        int32_t numBytes = convertFramesToBytes(frames1);
        memset(destination, 0, (size_t) numBytes);
        destination = &mStorage[0];
        int frames2 = framesToRead - frames1;
        numBytes = convertFramesToBytes(frames2);
        memset(destination, 0, (size_t) numBytes);
    } else {
        int32_t numBytes = convertFramesToBytes(framesToRead);
        memset(destination, 0, (size_t) numBytes);
    }
}

} // namespace oboe

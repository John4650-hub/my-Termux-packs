#include <stdint.h>
#include <time.h>
#include <iostream>
#include <memory.h>
#include <assert.h>
#include "oboe/FifoBuffer.h"
#include "oboe/customFifo.h"
#include "oboe/FifoControllerBase.h"
#include "oboe/Definitions.h"

namespace oboe {

	CustomFifoBuffer::CustomFifoBuffer(uint32_t bytesPerFrame, uint32_t capacityInFrames): FifoBuffer(bytesPerFrame, capacityInFrames) {}

    // Custom read method that also clears the elements after reading
    int32_t CustomFifoBuffer::read_custom(void* buffer, int32_t numFrames) {
        int32_t framesAvailable = mFifo->getFullFramesAvailable();
        int32_t framesToRead = numFrames;
        // Is there enough data in the FIFO
        if (framesToRead > framesAvailable) {
            framesToRead = framesAvailable;
        }
        if (framesToRead <= 0) {
            return 0;
        }

        uint32_t readIndex = mFifo->getReadIndex();
        uint8_t* destination = reinterpret_cast<uint8_t*>(buffer);
        uint8_t* source = &mStorage[convertFramesToBytes(readIndex)];
        if ((readIndex + framesToRead) > mFifo->getFrameCapacity()) {
            // read in two parts, first part here is at the end of the mStorage buffer
            int32_t frames1 = static_cast<int32_t>(mFifo->getFrameCapacity() - readIndex);
            int32_t numBytes = convertFramesToBytes(frames1);
            if (numBytes < 0) {
                return (int32_t) Result::ErrorOutOfRange;
            }
            memcpy(destination, source, (size_t) numBytes);
            destination += numBytes;
            // read second part, which is at the beginning of mStorage
            source = &mStorage[0];
            int frames2 = framesToRead - frames1;
            numBytes = convertFramesToBytes(frames2);
            if (numBytes < 0) {
                return (int32_t) Result::ErrorOutOfRange;
            }
            memcpy(destination, source, (size_t) numBytes);
        } else {
            // just read in one shot
            int32_t numBytes = convertFramesToBytes(framesToRead);
            if (numBytes < 0) {
                return (int32_t) Result::ErrorOutOfRange;
            }
            memcpy(destination, source, (size_t) numBytes);
        }
        clearAfterRead(readIndex, framesToRead); // Clear elements after reading
        mFifo->advanceReadIndex(framesToRead);

        return framesToRead;
    }

    // New method to clear elements after reading
    void CustomFifoBuffer::clearAfterRead(uint32_t readIndex, int32_t framesToRead) {
				int32_t numBytes{};
        uint8_t* destination = &mStorage[convertFramesToBytes(readIndex)];
        if ((readIndex + framesToRead) > mFifo->getFrameCapacity()) {
            // clear in two parts, first part here is at the end of the mStorage buffer
            int32_t frames1 = static_cast<int32_t>(mFifo->getFrameCapacity() - readIndex);
            numBytes = convertFramesToBytes(frames1);
            memset(destination, 0, (size_t) numBytes);
            // clear second part, which is at the beginning of mStorage
            destination = &mStorage[0];
            int frames2 = framesToRead - frames1;
            numBytes = convertFramesToBytes(frames2);
            memset(destination, 0, (size_t) numBytes);
        } else {
            // just clear in one shot
            numBytes = convertFramesToBytes(framesToRead);

            memset(destination, 0, (size_t) numBytes);
        }
				std::cout<<numBytes<<" cleared"<<std::endl;
    }

} // namespace oboe

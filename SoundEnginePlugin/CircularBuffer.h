#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <vector>
#include <algorithm>
#include <cstring>

template <typename SampleType>
class CircularBuffer {
private:
    std::vector<SampleType> mDelayBuffer;
    std::vector<AkReal32> mOutBuffer;
    AkUInt32 mWriteHead;
    AkUInt32 mReadHead;
    AkReal32 mDelayTime; 

public:
    CircularBuffer()
        : mWriteHead(0), mReadHead(0), mDelayTime(0.5) { }

    bool SetUp (AkUInt16 SampleRate, DSPLABFXParams* pCurrentParams)
    {
        mDelayTime = pCurrentParams->RTPC.fDelayTime;
        mDelayBuffer.resize(2 * SampleRate);
        mOutBuffer.resize(2 * SampleRate);
        mReadHead = (mWriteHead - (AkUInt32)(mDelayTime * SampleRate) + (AkUInt32)mDelayBuffer.size()) % (AkUInt32)mDelayBuffer.size();
        return true;
    }

    void Render(SampleType* AK_RESTRICT pInput, AkUInt16 validFrames)
    {
        for (AkUInt16 i = 0; i < validFrames; i++)
        {
            mDelayBuffer[mWriteHead] = pInput[i] + mOutBuffer[i]*0.6;
            mOutBuffer[i] = mDelayBuffer[mReadHead];
            mWriteHead++;
            if (mWriteHead >= mDelayBuffer.size())
                mWriteHead = 0;
            mReadHead++;
            if (mReadHead >= mDelayBuffer.size())
                mReadHead = 0;
            /*pInput[i] = mOutBuffer[i];*/
        }
        std::size_t sizeInBytes = mOutBuffer.size() * sizeof(AkReal32);
        memcpy(pInput, mOutBuffer.data(), sizeInBytes);
    }
};


#endif

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_


#include <vector>
#include <algorithm>
#include <cstring>


template <typename SampleType>
class CircularBuffer {
private:
    std::vector<SampleType> mDelayBuffer;
    std::vector<SampleType> mOutBuffer;
    AkUInt32 mWriteHead;
    AkUInt32 mReadHead;
    AkReal32 mDelayTime; 
    AkReal32 mFeedback;

public:
    CircularBuffer()
        : mWriteHead(0), mReadHead(0), mDelayTime(0.5), mFeedback(0.1) { }

    bool SetUp (AkUInt16 SampleRate, DSPLABFXParams* pCurrentParams)
    {
        mDelayTime = pCurrentParams->RTPC.fDelayTime;
        mFeedback = pCurrentParams->RTPC.fFeedback;
        mDelayBuffer.resize(2 * SampleRate);
        
        mReadHead = (mWriteHead - (AkUInt32)(mDelayTime * SampleRate) + (AkUInt32)mDelayBuffer.size()) % (AkUInt32)mDelayBuffer.size();
        return true;
    }

    void Render(SampleType* AK_RESTRICT pInput, AkUInt16 validFrames)
    {
        mOutBuffer.resize(validFrames);
        for (AkUInt16 i = 0; i < validFrames; i++)
        {
            mDelayBuffer[mWriteHead] = pInput[i] + mOutBuffer[i]* mFeedback;
            mOutBuffer[i] = mDelayBuffer[mReadHead];
            mWriteHead++;
            if (mWriteHead >= mDelayBuffer.size())
                mWriteHead = 0;
            mReadHead++;
            if (mReadHead >= mDelayBuffer.size())
                mReadHead = 0;
            /*pInput[i] = mOutBuffer[i];*/
        }
        std::size_t sizeInBytes = validFrames * sizeof(SampleType);
        // std::size_t sizeInBytes = (std::min)(mOutBuffer.size() * sizeof(SampleType), pInputSize * sizeof(SampleType));
        memcpy(pInput, mOutBuffer.data(), sizeInBytes);
    }
};

#endif

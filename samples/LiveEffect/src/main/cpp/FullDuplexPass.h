/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPLES_FULLDUPLEXPASS_H
#define SAMPLES_FULLDUPLEXPASS_H

#include "FullDuplexStream.h"

class FullDuplexPass : public FullDuplexStream {
public:
    virtual oboe::DataCallbackResult
    onBothStreamsReady(
            std::shared_ptr<oboe::AudioStream> inputStream,
            const void *inputData,
            int   numInputFrames,
            std::shared_ptr<oboe::AudioStream> outputStream,
            void *outputData,
            int   numOutputFrames) {
        if (!mEnableEarReturn) {
            return oboe::DataCallbackResult::Continue;
        }
        // The data format for both streams is Float.
        const float *inputFloats = static_cast<const float *>(inputData);
        float *outputFloats = static_cast<float *>(outputData);

        // It also assumes the channel count for each stream is the same.
        int32_t samplesPerFrame = outputStream->getChannelCount();
        int32_t numInputSamples = numInputFrames * samplesPerFrame;
        int32_t numOutputSamples = numOutputFrames * samplesPerFrame;

        // It is possible that there may be fewer input than output samples.
        int32_t samplesToProcess = std::min(numInputSamples, numOutputSamples);

        // Cache input data.
        float *cacheFloats = new float[samplesToProcess];

        // Copy input data.
        memcpy(outputFloats, inputFloats, sizeof(float) * samplesToProcess);
        memcpy(cacheFloats, inputFloats, sizeof(float) * samplesToProcess);

        // Push cache data to queue.
        CacheModel *cache = new CacheModel(cacheFloats, samplesToProcess);
        mCacheQueue.push(cache);

        // If there are fewer input samples then clear the rest of the buffer.
        int32_t samplesLeft = numOutputSamples - numInputSamples;
        if (samplesLeft > 0) {
            // silence
            memset(outputFloats + samplesToProcess, 0.0, sizeof(float) * samplesLeft);
        }

        return oboe::DataCallbackResult::Continue;
    }
};
#endif //SAMPLES_FULLDUPLEXPASS_H

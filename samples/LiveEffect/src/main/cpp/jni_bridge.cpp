/**
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

#include <jni.h>
#include <logging_macros.h>
#include "LiveEffectEngine.h"
#include <oboe/Utilities.h>
#include <oboe/AudioStream.h>

static const int kOboeApiAAudio = 0;
static const int kOboeApiOpenSLES = 1;

static LiveEffectEngine *engine = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_create(JNIEnv *env,
                                                               jclass) {
    if (engine == nullptr) {
        engine = new LiveEffectEngine();
    }

    return (engine != nullptr) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_delete(JNIEnv *env,
                                                               jclass) {
    if (engine) {
        engine->setEffectOn(false);
        delete engine;
        engine = nullptr;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_setEffectOn(
    JNIEnv *env, jclass, jboolean isEffectOn) {
    if (engine == nullptr) {
        LOGE(
            "Engine is null, you must call createEngine before calling this "
            "method");
        return JNI_FALSE;
    }

    return engine->setEffectOn(isEffectOn) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_setRecordingDeviceId(
    JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE(
            "Engine is null, you must call createEngine before calling this "
            "method");
        return;
    }

    engine->setRecordingDeviceId(deviceId);
}

JNIEXPORT void JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_setPlaybackDeviceId(
    JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE(
            "Engine is null, you must call createEngine before calling this "
            "method");
        return;
    }

    engine->setPlaybackDeviceId(deviceId);
}

JNIEXPORT jboolean JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_setAPI(JNIEnv *env,
                                                               jclass type,
                                                               jint apiType) {
    if (engine == nullptr) {
        LOGE(
            "Engine is null, you must call createEngine "
            "before calling this method");
        return JNI_FALSE;
    }

    oboe::AudioApi audioApi;
    switch (apiType) {
        case kOboeApiAAudio:
            audioApi = oboe::AudioApi::AAudio;
            break;
        case kOboeApiOpenSLES:
            audioApi = oboe::AudioApi::OpenSLES;
            break;
        default:
            LOGE("Unknown API selection to setAPI() %d", apiType);
            return JNI_FALSE;
    }

    return engine->setAudioApi(audioApi) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_isAAudioRecommended(
    JNIEnv *env, jclass type) {
    if (engine == nullptr) {
        LOGE(
            "Engine is null, you must call createEngine "
            "before calling this method");
        return JNI_FALSE;
    }
    return engine->isAAudioRecommended() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_native_1setDefaultStreamValues(JNIEnv *env,
                                               jclass type,
                                               jint sampleRate,
                                               jint framesPerBurst) {
    oboe::DefaultStreamValues::SampleRate = (int32_t) sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) framesPerBurst;
}

JNIEXPORT void JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_enableEarReturn(JNIEnv *env, jclass clazz,
                                                                         jboolean enable) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }
    engine->enableEarReturn(enable);
}

JNIEXPORT jint JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_read(JNIEnv *env, jclass clazz,
                                                              jshortArray outputArray) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }
    CacheModel *cache;
    bool success = engine->mFullDuplexPass.mCacheQueue.pop(cache);
    if (success) {
        // Copy cache data to output.
        int32_t size = cache->getSize();
        jshort *outputPtr = env->GetShortArrayElements(outputArray, JNI_FALSE);
        // Convert to pcm16.
        oboe::convertFloatToPcm16(cache->getData(), outputPtr, size);
        // Release resource.
        env->ReleaseShortArrayElements(outputArray, outputPtr, JNI_OK);
        delete cache;
        return size;
    }
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_getBitsPerSample(JNIEnv *env,
                                                                          jclass clazz) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }
    // 16位（Float类型也会转成pcm16）
    return 16;
}

JNIEXPORT jint JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_getSampleRate(JNIEnv *env, jclass clazz) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }
    return engine->mRecordingStream->getSampleRate();
}

JNIEXPORT jint JNICALL
Java_com_google_oboe_samples_liveEffect_LiveEffectEngine_getChannelCount(JNIEnv *env,
                                                                         jclass clazz) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }
    return engine->mRecordingStream->getChannelCount();
}

} // extern "C"

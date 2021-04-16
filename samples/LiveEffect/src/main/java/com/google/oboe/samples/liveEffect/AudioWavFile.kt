package com.google.oboe.samples.liveEffect

import java.io.DataOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.RandomAccessFile
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * Created by xuyang on 2021/1/21.
 */
class AudioWavFile(
    //采样位数
    private val bitsPerSample: Int,
    //采样率
    private val sampleRateInHz: Int,
    //声道数
    private val channelCount: Int,
    private val file: File
) {
    private var dataOutputStream: DataOutputStream? = null

    fun open() {
        dataOutputStream = DataOutputStream(FileOutputStream(file))
    }

    fun write(audioBytes: ByteArray) {
        dataOutputStream?.write(audioBytes)
    }

    fun write(audioShorts: ShortArray, offset: Int, length: Int) {
        if (offset >= audioShorts.size || length == 0) {
            return
        }
        var realLength = length
        if (offset + realLength > audioShorts.size) {
            realLength = audioShorts.size - offset
        }
        val byteBuffer = ByteBuffer.allocate(realLength * 2).order(ByteOrder.nativeOrder())
        for (i in offset until (offset + realLength)) {
            byteBuffer.putShort(audioShorts[i])
        }
        write(byteBuffer.array())
    }

    fun close() {
        if (dataOutputStream != null) {
            //写入wav header
            val random = RandomAccessFile(file, "rw")
            random.seek(0)
            random.write(wavHeader(dataOutputStream!!.size()))
        }
    }

    /**
     * http://soundfile.sapp.org/doc/WaveFormat/
     */
    private fun wavHeader(sizeOfBytes: Int): ByteArray {
        // 4 + (8 + Subchunk1Size) + (8 + Subchunk2Size)
        val totalSize = 4 + (8 + 16) + (8 + sizeOfBytes)
        // SampleRate * NumChannels * BitsPerSample / 8
        val byteRate: Int = sampleRateInHz * channelCount * bitsPerSample / 8
        // header
        val header = ByteArray(44)
        // "RIFF"
        header[0] = 'R'.toByte()
        header[1] = 'I'.toByte()
        header[2] = 'F'.toByte()
        header[3] = 'F'.toByte()

        // total size
        header[4] = (totalSize and 0xff).toByte()
        header[5] = (totalSize shr 8 and 0xff).toByte()
        header[6] = (totalSize shr 16 and 0xff).toByte()
        header[7] = (totalSize shr 24 and 0xff).toByte()

        // "WAVE"
        header[8] = 'W'.toByte()
        header[9] = 'A'.toByte()
        header[10] = 'V'.toByte()
        header[11] = 'E'.toByte()

        // "fmt "
        header[12] = 'f'.toByte()
        header[13] = 'm'.toByte()
        header[14] = 't'.toByte()
        header[15] = ' '.toByte()

        // Subchunk1Size (16 for PCM)
        header[16] = 16
        header[17] = 0
        header[18] = 0
        header[19] = 0

        // AudioFormat (PCM = 1)
        header[20] = 1
        header[21] = 0

        // NumChannels (Mono = 1, Stereo = 2)
        header[22] = channelCount.toByte()
        header[23] = 0

        // SampleRate
        header[24] = (sampleRateInHz and 0xff).toByte()
        header[25] = (sampleRateInHz shr 8 and 0xff).toByte()
        header[26] = (sampleRateInHz shr 16 and 0xff).toByte()
        header[27] = (sampleRateInHz shr 24 and 0xff).toByte()

        // ByteRate = SampleRate * NumChannels * BitsPerSample / 8
        header[28] = (byteRate and 0xff).toByte()
        header[29] = (byteRate shr 8 and 0xff).toByte()
        header[30] = (byteRate shr 16 and 0xff).toByte()
        header[31] = (byteRate shr 24 and 0xff).toByte()

        // BlockAlign = NumChannels * BitsPerSample / 8
        header[32] = (channelCount * bitsPerSample / 8).toByte()
        header[33] = 0

        // BitsPerSample
        header[34] = bitsPerSample.toByte() // bits per sample
        header[35] = 0

        // "data"
        header[36] = 'd'.toByte()
        header[37] = 'a'.toByte()
        header[38] = 't'.toByte()
        header[39] = 'a'.toByte()

        // Subchunk2Size (number of bytes)
        header[40] = (sizeOfBytes and 0xff).toByte()
        header[41] = (sizeOfBytes shr 8 and 0xff).toByte()
        header[42] = (sizeOfBytes shr 16 and 0xff).toByte()
        header[43] = (sizeOfBytes shr 24 and 0xff).toByte()

        return header
    }
}
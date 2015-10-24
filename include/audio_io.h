/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TIZEN_MEDIA_AUDIO_IO_H__
#define __TIZEN_MEDIA_AUDIO_IO_H__

#include <tizen.h>
#include <sound_manager.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file audio_io.h
 * @brief This file contains the Audio Input and Audio Output API.
 */

/**
 * @addtogroup CAPI_MEDIA_AUDIO_IN_MODULE
 * @{
 */

/**
 * @brief The audio input handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct audio_in_s *audio_in_h;

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_AUDIO_OUT_MODULE
 * @{
 */

/**
 * @brief The audio output handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct audio_out_s *audio_out_h;

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_AUDIO_IO_MODULE
 * @{
 */

/**
 * @brief Enumeration for audio sample type with bit depth.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    AUDIO_SAMPLE_TYPE_U8 = 0x70,    /**< Unsigned 8-bit audio samples */
    AUDIO_SAMPLE_TYPE_S16_LE,       /**< Signed 16-bit audio samples */
} audio_sample_type_e;

/**
 * @brief Enumeration for audio channel.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
    AUDIO_CHANNEL_MONO = 0x80,                  /**< 1 channel, mono */
    AUDIO_CHANNEL_STEREO,                       /**< 2 channel, stereo */
} audio_channel_e;

/**
 * @brief Enumeration for audio input and output error.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum{
    AUDIO_IO_ERROR_NONE                = TIZEN_ERROR_NONE,              /**< Successful */
    AUDIO_IO_ERROR_OUT_OF_MEMORY       = TIZEN_ERROR_OUT_OF_MEMORY,     /**< Out of memory */
    AUDIO_IO_ERROR_INVALID_PARAMETER   = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
    AUDIO_IO_ERROR_INVALID_OPERATION   = TIZEN_ERROR_INVALID_OPERATION, /**< Invalid operation */
    AUDIO_IO_ERROR_PERMISSION_DENIED   = TIZEN_ERROR_PERMISSION_DENIED, /**< Device open error by security */
    AUDIO_IO_ERROR_NOT_SUPPORTED       = TIZEN_ERROR_NOT_SUPPORTED,     /**< Not supported */
    AUDIO_IO_ERROR_DEVICE_NOT_OPENED   = TIZEN_ERROR_AUDIO_IO | 0x01,   /**< Device open error */
    AUDIO_IO_ERROR_DEVICE_NOT_CLOSED   = TIZEN_ERROR_AUDIO_IO | 0x02,   /**< Device close error */
    AUDIO_IO_ERROR_INVALID_BUFFER      = TIZEN_ERROR_AUDIO_IO | 0x03,   /**< Invalid buffer pointer */
    AUDIO_IO_ERROR_SOUND_POLICY        = TIZEN_ERROR_AUDIO_IO | 0x04,   /**< Sound policy error */
} audio_io_error_e;

/**
 * @brief Enumeration for audio IO interrupted messages.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    AUDIO_IO_INTERRUPTED_COMPLETED = 0,         /**< Interrupt completed */
    AUDIO_IO_INTERRUPTED_BY_MEDIA,              /**< Interrupted by a media application */
    AUDIO_IO_INTERRUPTED_BY_CALL,               /**< Interrupted by an incoming call */
    AUDIO_IO_INTERRUPTED_BY_EARJACK_UNPLUG,     /**< Interrupted by unplugging headphones */
    AUDIO_IO_INTERRUPTED_BY_RESOURCE_CONFLICT,  /**< Interrupted by a resource conflict */
    AUDIO_IO_INTERRUPTED_BY_ALARM,              /**< Interrupted by an alarm */
    AUDIO_IO_INTERRUPTED_BY_EMERGENCY,          /**< Interrupted by an emergency */
    AUDIO_IO_INTERRUPTED_BY_NOTIFICATION,       /**< Interrupted by a notification */
} audio_io_interrupted_code_e;

/**
 * @brief Called when audio input or output is interrupted.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] error_code The interrupted error code
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @see audio_in_set_interrupted_cb()
 * @see audio_out_set_interrupted_cb()
 * @see audio_in_unset_interrupted_cb()
 * @see audio_out_unset_interrupted_cb()
 */
typedef void (*audio_io_interrupted_cb)(audio_io_interrupted_code_e code, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_AUDIO_IN_MODULE
 * @{
 */

//
//AUDIO INPUT
//

/**
 * @brief Called when audio input data is available in asynchronous(event) mode.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks @a use audio_in_peek() to get audio in data inside callback, use audio_in_drop() after use of peeked data.
 *
 * @param[in] handle The handle to the audio input
 * @param[in] nbytes The amount of available audio in data which can be peeked.
 * @param[in] userdata The user data passed from the callback registration function
 *
 * @see audio_in_set_stream_cb()
 */
typedef void (*audio_in_stream_cb)(audio_in_h handle, size_t nbytes, void *userdata);

/**
 * @brief Creates an audio device instance and returns an input handle to record PCM (pulse-code modulation) data.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/recorder
 *
 * @details This function is used for audio input initialization.
 *
 * @remarks @a input must be released using audio_in_destroy().
 *
 * @param[in] sample_rate The audio sample rate in 8000[Hz] ~ 48000[Hz]
 * @param[in] channel The audio channel type (mono or stereo)
 * @param[in] type The type of audio sample (8- or 16-bit)
 * @param[out] input An audio input handle is created on success
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_PERMISSION_DENIED Permission denied
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_OPENED Device not opened
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @see audio_in_destroy()
 */
int audio_in_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type, audio_in_h *input);

/**
 * @brief Creates an audio loopback device instance and returns an input handle to record PCM (pulse-code modulation) data.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/recorder
 *
 * @details This function is used for audio loopback input initialization.
 *
 * @remarks @a input must be released using audio_in_destroy().
 *
 * @param[in] sample_rate The audio sample rate in 8000[Hz] ~ 48000[Hz]
 * @param[in] channel The audio channel type, mono, or stereo
 * @param[in] type The type of audio sample (8- or 16-bit)
 * @param[out] input An audio input handle will be created, if successful
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_PERMISSION_DENIED Permission denied
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_OPENED Device not opened
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @see audio_in_destroy()
 */
int audio_in_create_loopback(int sample_rate, audio_channel_e channel, audio_sample_type_e type , audio_in_h* input);

/**
 * @brief Releases the audio input handle and all its resources associated with an audio stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input to destroy
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_CLOSED Device not closed
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @see audio_in_create()
 */
int audio_in_destroy(audio_in_h input);

/**
 * @brief Prepares reading audio input by starting buffering of audio data from the device.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_in_unprepare()
 */
int audio_in_prepare(audio_in_h input);

/**
 * @brief Unprepares reading audio input by stopping buffering the audio data from the device.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @see audio_in_prepare()
 */
int audio_in_unprepare(audio_in_h input);

/**
 * @brief Flushes and discards buffered audio data from the input stream.
 *
 * @since_tizen 2.4
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 */
int audio_in_flush(audio_in_h input);

/**
 * @brief Reads audio data from the audio input buffer.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @param[out] buffer The PCM buffer address
 * @param[in] length The length of the PCM data buffer (in bytes)
 * @return The number of read bytes on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_BUFFER Invalid buffer pointer
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @pre audio_in_start_recording().
 */
int audio_in_read(audio_in_h input, void *buffer, unsigned int length);

/**
 * @brief Gets the size to be allocated for the audio input buffer.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @param[out] size The buffer size (in bytes, the maximum size is 1 MB)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @see audio_in_read()
 */
int audio_in_get_buffer_size(audio_in_h input, int *size);

/**
 * @brief Gets the sample rate of the audio input data stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @param[out] sample_rate The audio sample rate in Hertz (8000 ~ 48000)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 */
int audio_in_get_sample_rate(audio_in_h input, int *sample_rate);

/**
 * @brief Gets the channel type of the audio input data stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @details The audio channel type defines whether the audio is mono or stereo.
 *
 * @param[in] input The handle to the audio input
 * @param[out] channel The audio channel type
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 */
int audio_in_get_channel(audio_in_h input, audio_channel_e *channel);

/**
 * @brief Gets the sample audio format (8-bit or 16-bit) of the audio input data stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @param[out] type The audio sample type
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 */
int audio_in_get_sample_type(audio_in_h input, audio_sample_type_e *type);

/**
 * @brief Registers a callback function to be invoked when the audio input handle is interrupted or the interrupt is completed.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 * @post audio_io_interrupted_cb() will be invoked.
 *
 * @see audio_in_unset_interrupted_cb()
 * @see audio_io_interrupted_cb()
 */
int audio_in_set_interrupted_cb(audio_in_h input, audio_io_interrupted_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_in_set_interrupted_cb()
 */
int audio_in_unset_interrupted_cb(audio_in_h input);

/**
 * @brief Ignores session for input.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 */
int audio_in_ignore_session(audio_in_h input);

/**
 * @brief Sets an asynchronous(event) callback function to handle recording PCM (pulse-code modulation) data.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @details @a callback will be called when you can read a PCM data.
 * It might cause dead lock if change the state of audio handle in callback.
 * (ex: audio_in_destroy, audio_in_prepare, audio_in_unprepare)
 * Recommend to use as a VOIP only.
 * Recommend not to hold callback too long.(it affects latency)
 *
 * @remarks @a input must be created using audio_in_create().
 *
 * @param[in] input    An audio input handle
 * @param[in] callback notify stream callback when user can read data (#audio_in_stream_cb)
 * @param[in] userdata user data to be retrieved when callback is called
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_OPENED Device not opened
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_out_set_stream_cb()
 */
int audio_in_set_stream_cb(audio_in_h input, audio_in_stream_cb callback, void* userdata);

/**
 * @brief Unregisters the callback function.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] input The handle to the audio input
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_in_set_interrupted_cb()
 */
int audio_in_unset_stream_cb(audio_in_h input);

/**
 * @brief peek from audio in buffer
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @details This function works correctly only with read, write callback. Otherwise it won't operate as intended.
 *
 * @remarks @a Works only in asynchronous(event) mode. This will just retrieve buffer pointer from audio in buffer. Drop after use.
 *
 * @param[in] input The handle to the audio input
 * @param[out] buffer start buffer pointer of peeked audio in data
 * @param[in,out] length amount of audio in data to be peeked
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_in_drop()
 */
int audio_in_peek(audio_in_h input, const void **buffer, unsigned int *length);

/**
 * @brief drop peeked audio buffer.
 *
 * @details This function works correctly only with read, write callback. Otherwise it won't operate as intended.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks @a Works only in asynchronous(event) mode. This will remove audio in data from actual stream buffer. Use this if peeked data is not needed anymore.
 *
 * @param[in] input The handle to the audio input
 * @return 0 on success, otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #AUDIO_IO_ERROR_NOT_SUPPORTED Not supported
 *
 * @see audio_in_peek()
 */
int audio_in_drop(audio_in_h input);



//
// AUDIO OUTPUT
//

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_AUDIO_OUT_MODULE
 * @{
 */

/**
 * @brief Called when audio out data can be written in asynchronous(event) mode.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks @a use audio_out_write() to write pcm data inside this callback.
 * @param[in] handle The handle to the audio output
 * @param[in] nbytes The amount of audio in data which can be written.
 * @param[in] userdata The user data passed from the callback registration function
 *
 * @see audio_out_set_stream_cb()
 */
typedef void (*audio_out_stream_cb)(audio_out_h handle, size_t nbytes, void *userdata);

/**
 * @brief Creates an audio device instance and returns an output handle to play PCM (pulse-code modulation) data.

 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @details This function is used for audio output initialization.
 *
 * @remarks @a output must be released by audio_out_destroy().
 *
 * @param[in] sample_rate The audio sample rate in 8000[Hz] ~ 48000[Hz]
 * @param[in] channel The audio channel type (mono or stereo)
 * @param[in] type The type of audio sample (8- or 16-bit)
 * @param[in] sound_type The type of sound (#sound_type_e)
 * @param[out] output An audio output handle is created on success
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_OPENED Device not opened
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 *
 * @see audio_out_destroy()
 */
int audio_out_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type, sound_type_e sound_type, audio_out_h *output);

/**
 * @brief Releases the audio output handle, along with all its resources.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output to destroy
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_CLOSED Device not closed
 *
 * @see audio_out_create()
 */
int audio_out_destroy(audio_out_h output);

/**
 * @brief Prepares playing audio output, this must be called before audio_out_write().
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see audio_out_unprepare()
 */
int audio_out_prepare(audio_out_h output);

/**
 * @brief Unprepares playing audio output.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see audio_out_prepare()
 */
int audio_out_unprepare(audio_out_h output);

/**
 * @brief Drains buffered audio data from the output stream.
 *
 * @details This function waits until drains stream buffer completely. (e.g end of playback)
 *
 * @since_tizen 2.4
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see audio_out_flush()
 */
int audio_out_drain(audio_out_h output);

/**
 * @brief Flushes and discards buffered audio data from the output stream.
 *
 * @since_tizen 2.4
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see audio_out_drain()
 */
int audio_out_flush(audio_out_h output);

/**
 * @brief Starts writing the audio data to the device.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[in,out] buffer The PCM buffer address
 * @param[in] length The length of the PCM buffer (in bytes)
 * @return The written data size on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_BUFFER Invalid buffer pointer
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 */
int audio_out_write(audio_out_h output, void *buffer, unsigned int length);

/**
 * @brief Gets the size to be allocated for the audio output buffer.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[out] size The suggested buffer size (in bytes, the maximum size is 1 MB)
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #AUDIO_IO_ERROR_NONE Successful
 * @retval  #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see audio_out_write()
 */
int audio_out_get_buffer_size(audio_out_h output, int *size);

/**
 * @brief Gets the sample rate of the audio output data stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[out] sample_rate The audio sample rate in Hertz (8000 ~ 48000)
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #AUDIO_IO_ERROR_NONE Successful
 * @retval  #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int audio_out_get_sample_rate(audio_out_h output, int *sample_rate);

/**
 * @brief Gets the channel type of the audio output data stream.
 * @details The audio channel type defines whether the audio is mono or stereo.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[out] channel The audio channel type
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int audio_out_get_channel(audio_out_h output, audio_channel_e *channel);

/**
 * @brief Gets the sample audio format (8-bit or 16-bit) of the audio output data stream.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[out] type The audio sample type
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int audio_out_get_sample_type(audio_out_h output, audio_sample_type_e *type);


/**
 * @brief Gets the sound type supported by the audio output device.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[out] type The sound type
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int audio_out_get_sound_type(audio_out_h output, sound_type_e *type);

/**
 * @brief Registers a callback function to be invoked when the audio output handle is interrupted or the interrupt is completed.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 *
 * @post audio_io_interrupted_cb() will be invoked.
 * @see audio_out_unset_interrupted_cb()
 * @see audio_io_interrupted_cb()
 */
int audio_out_set_interrupted_cb(audio_out_h output, audio_io_interrupted_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see audio_out_set_interrupted_cb()
 */
int audio_out_unset_interrupted_cb(audio_out_h output);

/**
 * @brief Ignores session for output.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 */
int audio_out_ignore_session(audio_out_h output);

/**
 * @brief Sets an asynchronous(event) callback function to handle playing PCM (pulse-code modulation) data.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @details @a callback will be called when you can write a PCM data.
 * It might cause dead lock if change the state of audio handle in callback.
 * (ex: audio_in_destroy, audio_in_prepare, audio_in_unprepare)
 * Recommend to use as a VOIP only.
 * Recommend not to hold callback too long.(it affects latency)
 *
 * @remarks @a output must be created using audio_out_create().
 *
 * @param[in] output   An audio output handle
 * @param[in] callback notify stream callback when user can write data (#audio_out_stream_cb)
 * @param[in] userdata user data to be retrieved when callback is called
 * @return 0 on success, otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #AUDIO_IO_ERROR_DEVICE_NOT_OPENED Device not opened
 * @retval #AUDIO_IO_ERROR_SOUND_POLICY Sound policy error
 *
 * @see audio_in_set_stream_cb()
 */
int audio_out_set_stream_cb(audio_out_h output, audio_out_stream_cb callback, void* userdata);

/**
 * @brief Unregisters the callback function.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] output The handle to the audio output
 * @return 0 on success, otherwise a negative error value
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #AUDIO_IO_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see audio_out_set_stream_cb()
 */
int audio_out_unset_stream_cb(audio_out_h output);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MEDIA_AUDIO_IO_H__ */

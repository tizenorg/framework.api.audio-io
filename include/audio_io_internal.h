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

#ifndef __TIZEN_MEDIA_AUDIO_IO_INTERNAL_H__
#define __TIZEN_MEDIA_AUDIO_IO_INTERNAL_H__

#include <tizen.h>
#include <sound_manager.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file audio_io_internal.h
 * @brief This file contains the Audio Input and Audio Output API.
 */

/**
 * @brief    Gets the latency value of audio input data stream
 * @param[in]   input	The handle to the audio input
 * @param[out]  latency	The stream latency value(millisecond).
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval  #AUDIO_IO_ERROR_NONE Successful
 * @retval  #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @see audio_out_get_latency()
 */
int audio_in_get_latency(audio_in_h input, int *latency);

/**
 * @brief    Gets the latency value of audio output data stream
 * @param[in]   input	The handle to the audio output
 * @param[out]  latency	The stream latency value(millisecond).
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval  #AUDIO_IO_ERROR_NONE Successful
 * @retval  #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 * @see audio_in_get_latency()
 */
int audio_out_get_latency(audio_out_h output, int *latency);

/**
 * @brief    Set device which USB Mic input you want to capture
 * @remarks @a Can be used after audio_in_prepare
 * @param[in] input The handle to the audio input
 * @param[in] device Device unique information (udev id) to Capture
 * @return 0 on success, otherwise a negative error value.
 * @retval #AUDIO_IO_ERROR_NONE Successful
 * @retval #AUDIO_IO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int audio_in_set_device(audio_in_h input, char* device);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MEDIA_AUDIO_IO_INTERNAL_H__ */

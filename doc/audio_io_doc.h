/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_MEDIA_AUDIO_IO_DOC_H__
#define __TIZEN_MEDIA_AUDIO_IO_DOC_H__


/**
 * @file audio_io_doc.h
 * @brief This file contains high level documentation for the Audio I/O API.
 *
 */

/**
 * @defgroup CAPI_MEDIA_AUDIO_IO_MODULE Audio I/O
 * @ingroup CAPI_MEDIA_FRAMEWORK
 */

/**
 * @ingroup CAPI_MEDIA_FRAMEWORK
 * @addtogroup CAPI_MEDIA_AUDIO_IO_MODULE
 * @brief The @ref CAPI_MEDIA_AUDIO_IO_MODULE API provides functions for controlling audio devices.
 * @section CAPI_MEDIA_AUDIO_IO_MODULE_HEADER Required Header
 *    \#include <audio_io.h>
 *
 * @section CAPI_MEDIA_AUDIO_IO_MODULE_OVERVIEW Overview
 * The Audio I/O API provides a set of functions to directly manage the system audio devices.
 * It gives easy access to the hardware layer of the sound card with a professional multichannel audio interface.
 * It should be used for activities requiring raw audio data buffers(PCM format).
 *
 * Programming the interface requires first obtaining a handle to the device, via the audio_in_create() or audio_out_create() function.
 *
 * The input and output devices both have an available set of queries, to find the suggested buffer size, sampling rate, channel type,
 * and sample type. For output, there is an additional query, to get the sound type (these types are defined in the @ref CAPI_MEDIA_SOUND_MANAGER_MODULE API).
 *
 * Reading and writing is done by allocating a buffer and passing the buffer to the input device
 * via audio_in_start_recording(), audio_in_read(), or writing to the buffer and passing it to the output device via audio_out_write().
 *
 */

 /**
 * @ingroup CAPI_MEDIA_AUDIO_IO_MODULE
 * @defgroup CAPI_MEDIA_AUDIO_IN_MODULE Audio Input
 * @brief The @ref CAPI_MEDIA_AUDIO_IN_MODULE API provides a set of functions to directly manage the system audio input devices.
 * @section CAPI_MEDIA_AUDIO_IN_MODULE_HEADER Required Header
 *   \#include <audio_io.h>
 *
 * @section CAPI_MEDIA_AUDIO_IN_MODULE_OVERVIEW Overview
 * The Audio Input API provides a set of functions to record audio data (PCM format) from audio devices.
 *
 * @section CAPI_MEDIA_AUDIO_IN_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/microphone\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */

 /**
 * @ingroup CAPI_MEDIA_AUDIO_IO_MODULE
 * @defgroup CAPI_MEDIA_AUDIO_OUT_MODULE Audio Output
 * @brief The @ref CAPI_MEDIA_AUDIO_OUT_MODULE API provides a set of functions to directly manage the system audio output devices.
 * @section CAPI_MEDIA_AUDIO_OUT_MODULE_HEADER Required Header
 *   \#include <audio_io.h>
 *
 * @section CAPI_MEDIA_AUDIO_OUT_MODULE_OVERVIEW Overview
 * The Audio Output API provides a set of functions to play recorded audio data from Audio Input.
 *
 *
 */
#endif /* __TIZEN_MEDIA_AUDIO_IO_DOC_H__ */



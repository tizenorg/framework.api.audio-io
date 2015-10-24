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

#ifndef __TIZEN_MEDIA_AUDIO_IO_PRIVATE_H__
#define __TIZEN_MEDIA_AUDIO_IO_PRIVATE_H__

#include <sound_manager.h>
#include <mm_sound.h>
#include "audio_io.h"

/*
 * Internal Macros
 */

#define AUDIO_IO_CHECK_CONDITION(condition,error,msg)   \
                if(condition) {} else \
                { LOGE("[%s] %s(0x%08x)",__FUNCTION__, msg,error); return error;}; \

#define AUDIO_IO_NULL_ARG_CHECK(arg)	\
	AUDIO_IO_CHECK_CONDITION(arg != NULL, AUDIO_IO_ERROR_INVALID_PARAMETER, "AUDIO_IO_ERROR_INVALID_PARAMETER" )

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _audio_in_s{
	MMSoundPcmHandle_t mm_handle;
	int is_async;
	int is_loopback;
	int _buffer_size;
	int _sample_rate;
	audio_channel_e _channel;
	audio_sample_type_e _type;
	audio_io_interrupted_cb user_cb;
	void* user_data;
	audio_in_stream_cb stream_cb;
	void* stream_userdata;
} audio_in_s;

typedef struct _audio_out_s{
	MMSoundPcmHandle_t mm_handle;
	int is_async;
	int is_loopback;
	int _buffer_size;
	int _sample_rate;
	audio_channel_e _channel;
	audio_sample_type_e _type; 	
	sound_type_e	_sound_type;
	audio_io_interrupted_cb user_cb;
	void* user_data;
	audio_out_stream_cb stream_cb;
	void* stream_userdata;
} audio_out_s;

int __convert_audio_io_error_code(int code, char *func_name);
int __check_parameter(int sample_rate, audio_channel_e channel, audio_sample_type_e type);
int __mm_sound_pcm_capture_msg_cb (int message, void *param, void *user_param);
audio_io_interrupted_code_e __translate_interrupted_code (int code);

int audio_in_create_private(int sample_rate, audio_channel_e channel, audio_sample_type_e type , int source_type, audio_in_h* input);

int audio_in_set_callback_private(audio_in_h input, audio_in_stream_cb callback, void* userdata);

int audio_out_create_private(int sample_rate, audio_channel_e channel, audio_sample_type_e type, sound_type_e sound_type, audio_out_h* output);

int audio_out_set_callback_private(audio_out_h output, audio_out_stream_cb callback, void* userdata);


#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MEDIA_AUDIO_IO_PRIVATE_H__

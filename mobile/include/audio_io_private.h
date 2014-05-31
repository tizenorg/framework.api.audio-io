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
#define	__TIZEN_MEDIA_AUDIO_IO_PRIVATE_H__
#include <audio_io.h>
#include <sound_manager.h>
#include <mm_sound.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _audio_in_s{
	MMSoundPcmHandle_t mm_handle;
	int _buffer_size;
	int _sample_rate;
	audio_channel_e _channel;
	audio_sample_type_e _type;
	audio_io_interrupted_cb user_cb;
	void* user_data;
} audio_in_s;

typedef struct _audio_out_s{
	MMSoundPcmHandle_t mm_handle;
	int _buffer_size;
	int _sample_rate;
	audio_channel_e _channel;
	audio_sample_type_e _type; 	
	sound_type_e	_sound_type;
	audio_io_interrupted_cb user_cb;
	void* user_data;
} audio_out_s;

#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MEDIA_AUDIO_IO_PRIVATE_H__

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mm.h>
#include <glib.h>
#include <audio_io_private.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_AUDIO_IO"

/*
* Internal Macros
*/
#define AUDIO_IO_CHECK_CONDITION(condition,error,msg)	\
		if(condition) {} else \
		{ LOGE("[%s] %s(0x%08x)",__FUNCTION__, msg,error); return error;}; \

#define AUDIO_IO_NULL_ARG_CHECK(arg)	\
	AUDIO_IO_CHECK_CONDITION(arg != NULL, AUDIO_IO_ERROR_INVALID_PARAMETER, "AUDIO_IO_ERROR_INVALID_PARAMETER" )

/*
* Internal Implementation
*/
static int __convert_error_code(int code, char *func_name)
{
	int ret = AUDIO_IO_ERROR_NONE;
	char* msg = "AUDIO_IO_ERROR_NONE";

	switch(code)
	{
		case MM_ERROR_NONE:
			ret = AUDIO_IO_ERROR_NONE;
			msg = "AUDIO_IO_ERROR_NONE";
			break;
		case MM_ERROR_INVALID_ARGUMENT:
		case MM_ERROR_SOUND_DEVICE_INVALID_SAMPLERATE:
		case MM_ERROR_SOUND_DEVICE_INVALID_CHANNEL:
		case MM_ERROR_SOUND_DEVICE_INVALID_FORMAT:
			ret = AUDIO_IO_ERROR_INVALID_PARAMETER;
			msg = "AUDIO_IO_ERROR_INVALID_PARAMETER";
			break;
		case MM_ERROR_SOUND_DEVICE_NOT_OPENED:
			ret = AUDIO_IO_ERROR_DEVICE_NOT_OPENED;
			msg = "AUDIO_IO_ERROR_DEVICE_NOT_OPENED";
			break;
		case MM_ERROR_SOUND_INTERNAL:
			ret = AUDIO_IO_ERROR_DEVICE_NOT_CLOSED;
			msg = "AUDIO_IO_ERROR_DEVICE_NOT_CLOSED";
			break;
		case MM_ERROR_SOUND_INVALID_POINTER:
			ret = AUDIO_IO_ERROR_INVALID_BUFFER;
			msg = "AUDIO_IO_ERROR_INVALID_BUFFER";
			break;		
		case MM_ERROR_POLICY_BLOCKED:
		case MM_ERROR_POLICY_INTERRUPTED:
		case MM_ERROR_POLICY_INTERNAL:
		case MM_ERROR_POLICY_DUPLICATED:
			ret = AUDIO_IO_ERROR_SOUND_POLICY;
			msg = "AUDIO_IO_ERROR_SOUND_POLICY";
			break;
	} 
	LOGE("[%s] %s(0x%08x) : core fw error(0x%x)",func_name,msg, ret, code);
	return ret;	
}

static int __check_parameter(int sample_rate, audio_channel_e channel, audio_sample_type_e type)
{
	if(sample_rate<8000 || sample_rate > 48000)	{
		LOGE("[%s] AUDIO_IO_ERROR_INVALID_PARAMETER(0x%08x) :  Invalid sample rate (8000~48000Hz) : %d",__FUNCTION__, AUDIO_IO_ERROR_INVALID_PARAMETER,sample_rate);
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	}
	if (channel < AUDIO_CHANNEL_MONO || channel > AUDIO_CHANNEL_STEREO) {
		LOGE("[%s] AUDIO_IO_ERROR_INVALID_PARAMETER(0x%08x) :  Invalid audio channel : %d",__FUNCTION__, AUDIO_IO_ERROR_INVALID_PARAMETER,channel);
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	}
	if (type < AUDIO_SAMPLE_TYPE_U8 || type > AUDIO_SAMPLE_TYPE_S16_LE) {
		LOGE("[%s] AUDIO_IO_ERROR_INVALID_PARAMETER(0x%08x) :  Invalid sample typel : %d",__FUNCTION__, AUDIO_IO_ERROR_INVALID_PARAMETER,type);
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	}
	return AUDIO_IO_ERROR_NONE;
}


static audio_io_interrupted_code_e __translate_interrupted_code (int code)
{
	audio_io_interrupted_code_e e = AUDIO_IO_INTERRUPTED_COMPLETED;

	switch(code)
	{
	case MM_MSG_CODE_INTERRUPTED_BY_ALARM_END:
	case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_END:
		e = AUDIO_IO_INTERRUPTED_COMPLETED;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_MEDIA:
	case MM_MSG_CODE_INTERRUPTED_BY_OTHER_PLAYER_APP:
		e = AUDIO_IO_INTERRUPTED_BY_MEDIA;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_CALL_START:
		e = AUDIO_IO_INTERRUPTED_BY_CALL;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_EARJACK_UNPLUG:
		e = AUDIO_IO_INTERRUPTED_BY_EARJACK_UNPLUG;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_RESOURCE_CONFLICT:
		e = AUDIO_IO_INTERRUPTED_BY_RESOURCE_CONFLICT;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_ALARM_START:
		e = AUDIO_IO_INTERRUPTED_BY_ALARM;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_START:
		e = AUDIO_IO_INTERRUPTED_BY_EMERGENCY;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_RESUMABLE_MEDIA:
		e = AUDIO_IO_INTERRUPTED_BY_RESUMABLE_MEDIA;
		break;
	}

	return e;
}

static int __mm_sound_pcm_capture_msg_cb (int message, void *param, void *user_param)
{
	audio_io_interrupted_code_e e = AUDIO_IO_INTERRUPTED_COMPLETED;
	audio_in_s *handle = (audio_in_s *) user_param;
	MMMessageParamType *msg = (MMMessageParamType*)param;

	LOGI("[%s] Got message type : 0x%x with code : %d" ,__FUNCTION__, message, msg->code);

	if (handle->user_cb == NULL) {
		LOGI("[%s] No interrupt callback is set. Skip this" ,__FUNCTION__);
		return 0;
	}

	if (message == MM_MESSAGE_SOUND_PCM_INTERRUPTED) {
		e = __translate_interrupted_code (msg->code);
	} else if (message == MM_MESSAGE_SOUND_PCM_CAPTURE_RESTRICTED) {
		/* TODO : handling restricted code is needed */
		/* e = _translate_restricted_code (msg->code); */
	}

	handle->user_cb (e, handle->user_data);

	return 0;
}

static int __mm_sound_pcm_playback_msg_cb (int message, void *param, void *user_param)
{
	audio_io_interrupted_code_e e = AUDIO_IO_INTERRUPTED_COMPLETED;
	audio_out_s *handle = (audio_out_s *) user_param;
	MMMessageParamType *msg = (MMMessageParamType*)param;

	LOGI("[%s] Got message type : 0x%x with code : %d" ,__FUNCTION__, message, msg->code);

	if (handle->user_cb == NULL) {
		LOGI("[%s] No interrupt callback is set. Skip this" ,__FUNCTION__);
		return 0;
	}

	if (message == MM_MESSAGE_SOUND_PCM_INTERRUPTED) {
		e = __translate_interrupted_code (msg->code);
	}

	handle->user_cb (e, handle->user_data);

	return 0;
}


/*
* Public Implementation
*/

/* Audio In */
int audio_in_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type , audio_in_h* input)
{
	int ret = 0;
	audio_in_s *handle = NULL;

	/* input condition check */
	AUDIO_IO_NULL_ARG_CHECK(input);
	if(__check_parameter(sample_rate, channel, type) != AUDIO_IO_ERROR_NONE)
		return AUDIO_IO_ERROR_INVALID_PARAMETER;

	/* Create Handle */
	handle = (audio_in_s*)malloc( sizeof(audio_in_s));
	if (handle != NULL) {
		memset(handle, 0, sizeof(audio_in_s));
	} else {
		LOGE("[%s] ERROR :  AUDIO_IO_ERROR_OUT_OF_MEMORY(0x%08x)" ,__FUNCTION__, AUDIO_IO_ERROR_OUT_OF_MEMORY );
		return AUDIO_IO_ERROR_OUT_OF_MEMORY;
	}

	ret = mm_sound_pcm_capture_open( &handle->mm_handle,sample_rate, channel, type);
	if( ret < 0) {
		free (handle);
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_capture_open() success",__FUNCTION__);

	/* Fill information */
	*input = (audio_in_h)handle;
	handle->_buffer_size= ret;
	handle->_sample_rate= sample_rate;
	handle->_channel= channel;
	handle->_type= type;

	/* Set message interrupt callback */
	ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_capture_msg_cb, handle);
	if( ret < 0) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_set_message_callback() success",__FUNCTION__);

	return AUDIO_IO_ERROR_NONE;
}

int audio_in_destroy(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;

	int ret = mm_sound_pcm_capture_close(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	free(handle);

	LOGI("[%s] mm_sound_pcm_capture_close() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_prepare(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;

	int ret = mm_sound_pcm_capture_start(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_capture_start() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_unprepare(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;

	int ret = mm_sound_pcm_capture_stop(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_capture_stop() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_read(audio_in_h input, void *buffer, unsigned int length )
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(buffer);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = 0;
	int result = 0;

	ret = mm_sound_pcm_capture_read(handle->mm_handle, (void*) buffer, length);
	if (ret > 0) {
		LOGI("[%s] (%d/%d) bytes read" ,__FUNCTION__, ret, length);
		return ret;
	}

	switch(ret)
	{
		case MM_ERROR_SOUND_INVALID_STATE:
			result = AUDIO_IO_ERROR_INVALID_OPERATION;
			LOGE("[%s] (0x%08x) : Not recording started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
			break;
		default:
			result = __convert_error_code(ret, (char*)__FUNCTION__);
			break;
	}
	return result;
}

int audio_in_get_buffer_size(audio_in_h input, int *size)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(size);
	audio_in_s *handle = (audio_in_s *) input;

	*size = handle->_buffer_size;

	LOGI("[%s] buffer size = %d",__FUNCTION__, *size);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_get_sample_rate(audio_in_h input, int *sample_rate)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(sample_rate);
	audio_in_s *handle = (audio_in_s *) input;

	*sample_rate = handle->_sample_rate;

	LOGI("[%s] sample rate = %d",__FUNCTION__, *sample_rate);
	return AUDIO_IO_ERROR_NONE;
}


int audio_in_get_channel(audio_in_h input, audio_channel_e *channel)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(channel);
	audio_in_s *handle = (audio_in_s *) input;

	*channel = handle->_channel;

	LOGI("[%s] channel = %d",__FUNCTION__, *channel);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_get_sample_type(audio_in_h input, audio_sample_type_e *type)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(type);
	audio_in_s *handle = (audio_in_s *) input;

	*type = handle->_type;

	LOGI("[%s] sample type = %d",__FUNCTION__, *type);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_set_interrupted_cb(audio_in_h input, audio_io_interrupted_cb callback, void *user_data)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(callback);
	audio_in_s *handle = (audio_in_s *) input;

	handle->user_cb = callback;
	handle->user_data = user_data;

	LOGI("[%s] current interrupted cb (%p) / data (%p)",__FUNCTION__, handle->user_cb, handle->user_data);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_unset_interrupted_cb(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s  * handle = (audio_in_s  *) input;

	handle->user_cb = NULL;
	handle->user_data = NULL;

	LOGI("[%s] current interrupted cb (%p) / data (%p)",__FUNCTION__, handle->user_cb, handle->user_data);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_ignore_session(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s  * handle = (audio_in_s  *) input;
	int ret = 0;

	ret = mm_sound_pcm_capture_ignore_session(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_capture_ignore_session() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

/* Audio Out */
int audio_out_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type, sound_type_e sound_type,  audio_out_h* output)
{
	audio_out_s *handle = NULL;
	int ret = 0;

	/* input condition check */
	AUDIO_IO_NULL_ARG_CHECK(output);
	if(__check_parameter(sample_rate, channel, type)!=AUDIO_IO_ERROR_NONE)
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	if(sound_type < SOUND_TYPE_SYSTEM || sound_type > SOUND_TYPE_CALL) {
		LOGE("[%s] ERROR :  AUDIO_IO_ERROR_INVALID_PARAMETER(0x%08x) : Invalid sample sound type : %d" ,__FUNCTION__,AUDIO_IO_ERROR_INVALID_PARAMETER,sound_type );
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	}
	
	/* Create Handle */
	handle = (audio_out_s*)malloc( sizeof(audio_out_s));
	if (handle != NULL) {
		memset(handle, 0 , sizeof(audio_out_s));
	} else {
		LOGE("[%s] ERROR :  AUDIO_IO_ERROR_OUT_OF_MEMORY(0x%08x)" ,__FUNCTION__,AUDIO_IO_ERROR_OUT_OF_MEMORY );
		return AUDIO_IO_ERROR_OUT_OF_MEMORY;
	}
	ret = mm_sound_pcm_play_open(&handle->mm_handle,sample_rate, channel, type, sound_type);
	if( ret < 0) {
		free (handle);
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_play_open() success",__FUNCTION__);

	/* Fill information */
	*output = (audio_out_h)handle;
	handle->_buffer_size = ret;
	handle->_sample_rate = sample_rate;
	handle->_channel = channel;
	handle->_type = type;
	handle->_sound_type = sound_type;

	/* Set message interrupt callback */
	ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_playback_msg_cb, handle);
	if( ret < 0) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_set_message_callback() success",__FUNCTION__);

	return AUDIO_IO_ERROR_NONE;
}

int audio_out_destroy(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;

	int ret = mm_sound_pcm_play_close(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	free(handle);

	LOGI("[%s] mm_sound_pcm_play_close() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_prepare(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;

	int ret = mm_sound_pcm_play_start(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_play_start() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_unprepare(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;

	int ret = mm_sound_pcm_play_stop(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_play_stop() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_write(audio_out_h output, void* buffer, unsigned int length)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(buffer);
	audio_out_s *handle = (audio_out_s *) output;

	int ret = mm_sound_pcm_play_write(handle->mm_handle, (void*) buffer, length);
	if (ret > 0) {
		LOGI("[%s] (%d/%d) bytes written" ,__FUNCTION__, ret, length);
		return ret;
	}
	switch(ret)
	{
		case MM_ERROR_SOUND_INVALID_STATE:
			ret = AUDIO_IO_ERROR_INVALID_OPERATION;
			LOGE("[%s] (0x%08x) : Not playing started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
			break;
		default:
			ret = __convert_error_code(ret, (char*)__FUNCTION__);
			break;
	}
	return ret;
}


int audio_out_get_buffer_size(audio_out_h output, int *size)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(size);
	audio_out_s *handle = (audio_out_s *) output;

	*size = handle->_buffer_size;

	LOGI("[%s] buffer size = %d",__FUNCTION__, *size);
	return AUDIO_IO_ERROR_NONE;
}


int audio_out_get_sample_rate(audio_out_h output, int *sample_rate)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(sample_rate);
	audio_out_s *handle = (audio_out_s *) output;

	*sample_rate = handle->_sample_rate;

	LOGI("[%s] sample rate = %d",__FUNCTION__, *sample_rate);
	return AUDIO_IO_ERROR_NONE;
}


int audio_out_get_channel(audio_out_h output, audio_channel_e *channel)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(channel);
	audio_out_s *handle = (audio_out_s *) output;

	*channel = handle->_channel;

	LOGI("[%s] channel = %d",__FUNCTION__, *channel);
	return AUDIO_IO_ERROR_NONE;
}


int audio_out_get_sample_type(audio_out_h output, audio_sample_type_e *type)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(type);
	audio_out_s *handle = (audio_out_s *) output;

	*type = handle->_type;

	LOGI("[%s] sample type = %d",__FUNCTION__, *type);
	return AUDIO_IO_ERROR_NONE;
}


int audio_out_get_sound_type(audio_out_h output, sound_type_e *type)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(type);
	audio_out_s *handle = (audio_out_s *) output;

	*type = handle->_sound_type;

	LOGI("[%s] sound type = %d",__FUNCTION__, *type);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_set_interrupted_cb(audio_out_h output, audio_io_interrupted_cb callback, void *user_data)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(callback);
	audio_out_s *handle = (audio_out_s *) output;

	handle->user_cb = callback;
	handle->user_data = user_data;

	LOGI("[%s] current interrupted cb (%p) / data (%p)",__FUNCTION__, handle->user_cb, handle->user_data);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_unset_interrupted_cb(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;

	handle->user_cb = NULL;
	handle->user_data = NULL;

	LOGI("[%s] current interrupted cb (%p) / data (%p)",__FUNCTION__, handle->user_cb, handle->user_data);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_ignore_session(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;
	int ret = 0;

	ret = mm_sound_pcm_play_ignore_session(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_play_ignore_session() success",__FUNCTION__);

	return AUDIO_IO_ERROR_NONE;
}

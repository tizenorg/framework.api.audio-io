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
#include "audio_io_private.h"
#include <dlog.h>

#include <mm_sound_pcm_async.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_AUDIO_IO"

/*
 * Internal Implementation
 */

int __convert_audio_io_error_code(int code, char *func_name)
{
	int ret = AUDIO_IO_ERROR_INVALID_OPERATION;
	char* msg = "AUDIO_IO_ERROR_INVALID_OPERATION";

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
		case MM_ERROR_SOUND_PERMISSION_DENIED:
			ret = AUDIO_IO_ERROR_PERMISSION_DENIED;
			msg = "AUDIO_IO_ERROR_PERMISSION_DENIED";
			break;
		case MM_ERROR_SOUND_INTERNAL:
			ret = AUDIO_IO_ERROR_INVALID_OPERATION;
			msg = "AUDIO_IO_ERROR_INVALID_OPERATION";
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
	if(ret != AUDIO_IO_ERROR_NONE) {
		LOGE("[%s] %s(0x%08x) : core fw error(0x%x)",func_name,msg, ret, code);
	}
	return ret;
}

int __check_parameter(int sample_rate, audio_channel_e channel, audio_sample_type_e type)
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

//LCOV_EXCL_START
audio_io_interrupted_code_e __translate_interrupted_code (int code)
{
	audio_io_interrupted_code_e e = AUDIO_IO_INTERRUPTED_COMPLETED;

	switch(code)
	{
	case MM_MSG_CODE_INTERRUPTED_BY_CALL_END:
	case MM_MSG_CODE_INTERRUPTED_BY_ALARM_END:
	case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_END:
	case MM_MSG_CODE_INTERRUPTED_BY_NOTIFICATION_END:
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

	case MM_MSG_CODE_INTERRUPTED_BY_NOTIFICATION_START:
		e = AUDIO_IO_INTERRUPTED_BY_NOTIFICATION;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_START:
		e = AUDIO_IO_INTERRUPTED_BY_EMERGENCY;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_RESUMABLE_MEDIA:
		e = AUDIO_IO_INTERRUPTED_BY_RESUMABLE_MEDIA;
		break;

	case MM_MSG_CODE_INTERRUPTED_BY_RESUMABLE_CANCELED:
		e = AUDIO_IO_INTERRUPTED_BY_RESUMABLE_CANCELED;
		break;
	}

	return e;
}

int __mm_sound_pcm_capture_msg_cb (int message, void *param, void *user_param)
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
//LCOV_EXCL_STOP

static int __audio_in_stream_cb (void* p, int nbytes, void* userdata)
{
	audio_in_s *handle = (audio_in_s *) userdata;

#ifdef _AUDIO_IO_DEBUG_TIMING_
	LOGI("<< p=%p, nbytes=%d, userdata=%p", p, nbytes, userdata);
#endif

	if (handle && handle->stream_cb) {
		handle->stream_cb ((audio_in_h)handle, nbytes, handle->stream_userdata);
#ifdef _AUDIO_IO_DEBUG_TIMING_
		LOGI("<< handle->stream_cb(handle:%p, nbytes:%d, handle->stream_userdata:%p)", p, nbytes, userdata);
#endif
	} else {
		LOGI("No stream callback is set. Skip this");
	}
	return 0;
}

static int __audio_out_stream_cb (void* p, int nbytes, void* userdata)
{
	audio_out_s *handle = (audio_out_s *) userdata;
	bool is_started = false;
	char * dummy = NULL;

#ifdef _AUDIO_IO_DEBUG_TIMING_
	LOGI(">> p=%p, nbytes=%d, userdata=%p", p, nbytes, userdata);
#endif

	if (handle) {
		mm_sound_pcm_is_started_async(handle->mm_handle, &is_started);
		if (is_started) {
			if (handle->stream_cb) {
				handle->stream_cb ((audio_out_h)handle, nbytes, handle->stream_userdata);
			} else {
				LOGI("Started state but No stream callback is set. Skip this");
			}
		} else {
			LOGI("Not started....write dummy data");
			if ((dummy = (char*)malloc(nbytes)) != NULL) {
				memset (dummy, 0, nbytes);
				mm_sound_pcm_play_write_async(handle->mm_handle, (void*) dummy, nbytes);
				free (dummy);
				LOGI("write done!!!");
			} else {
				LOGE("ERROR :  AUDIO_IO_ERROR_OUT_OF_MEMORY(0x%08x)", AUDIO_IO_ERROR_OUT_OF_MEMORY);
			}
		}
	} else {
		LOGE("Handle is invalid...");
	}

	return 0;
}

int audio_in_create_private(int sample_rate, audio_channel_e channel, audio_sample_type_e type , int source_type, audio_in_h* input)
{
	int ret = 0;
	audio_in_s *handle = NULL;

	/* input condition check */
	AUDIO_IO_NULL_ARG_CHECK(input);
	if(__check_parameter(sample_rate, channel, type) != AUDIO_IO_ERROR_NONE)
		return AUDIO_IO_ERROR_INVALID_PARAMETER;

	/* Create Handle & Fill information */
	if ((handle = (audio_in_s*)malloc( sizeof(audio_in_s))) == NULL) {
		LOGE("ERROR :  AUDIO_IO_ERROR_OUT_OF_MEMORY(0x%08x)", AUDIO_IO_ERROR_OUT_OF_MEMORY);
		return AUDIO_IO_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0, sizeof(audio_in_s));

	/* Capture open */
	if ((ret = mm_sound_pcm_capture_open_ex(&handle->mm_handle, sample_rate, channel, type, source_type)) < 0) {
		LOGE("mm_sound_pcm_capture_open_ex() failed [0x%x]", ret);
		goto ERROR;
	}
	LOGI("mm_sound_pcm_capture_open_ex() success");


	if (source_type == SUPPORT_SOURCE_TYPE_LOOPBACK)
	{
	    handle->is_loopback = 1;
	}

	handle->_buffer_size = ret; /* return by pcm_open */
	handle->_sample_rate = sample_rate;
	handle->_channel     = channel;
	handle->_type        = type;

	/* Set message interrupt callback */
	if ((ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_capture_msg_cb, handle)) < 0) {
		LOGE("mm_sound_pcm_set_message_callback() failed [0x%x]", ret);
		goto ERROR;
	}
	LOGI("mm_sound_pcm_set_message_callback() success");

	/* Handle assign */
	*input = (audio_in_h)handle;

	return AUDIO_IO_ERROR_NONE;

ERROR:
	if (handle)
		free (handle);
	return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
}

int audio_in_set_callback_private(audio_in_h input, audio_in_stream_cb callback, void* userdata)
{
	AUDIO_IO_NULL_ARG_CHECK(input);

	int         ret         = AUDIO_IO_ERROR_NONE;
	int         source_type = SUPPORT_SOURCE_TYPE_DEFAULT;
	audio_in_s* handle      = (audio_in_s*)input;

	// at first, release existing audio handle
	if (handle->is_async) {
		ret = mm_sound_pcm_capture_close_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_capture_close(handle->mm_handle);
	}

	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	// Initialize flags
	handle->stream_cb       = NULL;
	handle->stream_userdata = NULL;
	handle->is_async        = 0;

	// Checks loopback type
	if (handle->is_loopback == 1) {
		source_type = SUPPORT_SOURCE_TYPE_LOOPBACK;
	}

	/* Async (callback exists) or Sync (otherwise) */
	if (callback != NULL) {
		handle->stream_cb       = callback;
		handle->stream_userdata = userdata;
		handle->is_async        = 1;

		/* Capture open */
		if ((ret = mm_sound_pcm_capture_open_async(&handle->mm_handle, handle->_sample_rate, handle->_channel, handle->_type, source_type,
				(mm_sound_pcm_stream_cb_t)__audio_in_stream_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_capture_open_async() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_capture_open_async() success");

		/* Set message interrupt callback */
		if ((ret = mm_sound_pcm_set_message_callback_async(handle->mm_handle, __mm_sound_pcm_capture_msg_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_set_message_callback_async() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_set_message_callback_async() success");
	} else {
		/* Capture open */
		if ((ret = mm_sound_pcm_capture_open_ex(&handle->mm_handle, handle->_sample_rate, handle->_channel, handle->_type, source_type)) < 0) {
			LOGE("mm_sound_pcm_capture_open_ex() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_capture_open_ex() success");

		/* Set message interrupt callback */
		if ((ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_capture_msg_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_set_message_callback() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_set_message_callback() success");
	}

	handle->_buffer_size = ret; /* return by pcm_open */

	return AUDIO_IO_ERROR_NONE;
}

int audio_out_create_private(int sample_rate, audio_channel_e channel, audio_sample_type_e type, sound_type_e sound_type, audio_out_h* output)
{
	audio_out_s *handle = NULL;
	int ret = 0;

	/* input condition check */
	AUDIO_IO_NULL_ARG_CHECK(output);
	if(__check_parameter(sample_rate, channel, type)!=AUDIO_IO_ERROR_NONE)
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	if(sound_type < SOUND_TYPE_SYSTEM || sound_type > SOUND_TYPE_VOICE) {
		LOGE("ERROR :  AUDIO_IO_ERROR_INVALID_PARAMETER(0x%08x) : Invalid sample sound type : %d",
				AUDIO_IO_ERROR_INVALID_PARAMETER,sound_type );
		return AUDIO_IO_ERROR_INVALID_PARAMETER;
	}

	/* Create Handle & Fill information */
	if ((handle = (audio_out_s*)malloc( sizeof(audio_out_s))) == NULL) {
		LOGE("ERROR :  AUDIO_IO_ERROR_OUT_OF_MEMORY(0x%08x)", AUDIO_IO_ERROR_OUT_OF_MEMORY);
		return AUDIO_IO_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0 , sizeof(audio_out_s));

	if ((ret = mm_sound_pcm_play_open(&handle->mm_handle,sample_rate, channel, type, sound_type)) < 0) {
		LOGE("mm_sound_pcm_play_open() failed [0x%x]", ret);
		goto ERROR;
	}
	LOGI("mm_sound_pcm_play_open() success");

	handle->_buffer_size = ret; /* return by pcm_open */
	handle->_sample_rate = sample_rate;
	handle->_channel     = channel;
	handle->_type        = type;
	handle->_sound_type  = sound_type;

	/* Set message interrupt callback */
	if ((ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_playback_msg_cb, handle)) < 0) {
		LOGE("mm_sound_pcm_set_message_callback() failed [0x%x]", ret);
		goto ERROR;
	}
	LOGI("mm_sound_pcm_set_message_callback() success");

	/* Handle assign */
	*output = (audio_out_h)handle;

	return AUDIO_IO_ERROR_NONE;

ERROR:
	if (handle)
		free (handle);
	return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
}

int audio_out_set_callback_private(audio_out_h output, audio_out_stream_cb callback, void* userdata)
{
	AUDIO_IO_NULL_ARG_CHECK(output);

	int          ret    = AUDIO_IO_ERROR_NONE;
	audio_out_s* handle = (audio_out_s*)output;

	// at first, release existing mm handle
	if (handle->is_async) {
		ret = mm_sound_pcm_play_close_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_play_close(handle->mm_handle);
	}

	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	// Initialize flags
	handle->stream_cb       = NULL;
	handle->stream_userdata = NULL;
	handle->is_async        = 0;

	/* Async (callback exists) or Sync (otherwise) */
	if (callback != NULL) {
		handle->stream_cb       = callback;
		handle->stream_userdata = userdata;
		handle->is_async        = 1;

		/* Playback open */
		if ((ret = mm_sound_pcm_play_open_async(&handle->mm_handle, handle->_sample_rate, handle->_channel, handle->_type, handle->_sound_type,
				(mm_sound_pcm_stream_cb_t)__audio_out_stream_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_play_open_async() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_play_open_async() success");

		/* Set message interrupt callback */
		if ((ret = mm_sound_pcm_set_message_callback_async(handle->mm_handle, __mm_sound_pcm_playback_msg_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_set_message_callback_async() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_set_message_callback_async() success");

	} else {
		if ((ret = mm_sound_pcm_play_open(&handle->mm_handle, handle->_sample_rate, handle->_channel, handle->_type, handle->_sound_type)) < 0) {
			LOGE("mm_sound_pcm_play_open() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_play_open() success");

		/* Set message interrupt callback */
		if ((ret = mm_sound_pcm_set_message_callback(handle->mm_handle, __mm_sound_pcm_playback_msg_cb, handle)) < 0) {
			LOGE("mm_sound_pcm_set_message_callback() failed [0x%x]", ret);
			return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		}
		LOGI("mm_sound_pcm_set_message_callback() success");
	}

	handle->_buffer_size = ret; /* return by pcm_open */

	return AUDIO_IO_ERROR_NONE;
}

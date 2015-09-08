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

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_AUDIO_IO"

#include <mm_sound_pcm_async.h>

/*
 * Internal Implementation
 */

/*
 * Public Implementation
 */

/* Audio In */
int audio_in_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type , audio_in_h* input)
{
	return audio_in_create_private (sample_rate, channel, type, SUPPORT_SOURCE_TYPE_DEFAULT, input);
}

int audio_in_create_loopback(int sample_rate, audio_channel_e channel, audio_sample_type_e type, audio_in_h* input)
{
	return audio_in_create_private (sample_rate, channel, type, SUPPORT_SOURCE_TYPE_LOOPBACK, input);
}

int audio_in_destroy(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_capture_close_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_capture_close(handle->mm_handle);
	}
	if (ret != MM_ERROR_NONE) {
		free(handle);
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}
	free(handle);

	LOGI("[%s] mm_sound_pcm_capture_close() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_prepare(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_capture_start_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_capture_start(handle->mm_handle);
	}

	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_capture_start() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_unprepare(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_capture_stop_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_capture_stop(handle->mm_handle);
	}
	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
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

	if (handle->is_async) {
		LOGE ("audio_in_read doesn't operate in async mode!!!, use audio_in_peek/audio_in_drop instead");
		return AUDIO_IO_ERROR_INVALID_OPERATION;
	}

	ret = mm_sound_pcm_capture_read(handle->mm_handle, (void*) buffer, length);
	if (ret > 0)
		return ret;

	switch(ret)
	{
		case MM_ERROR_SOUND_INVALID_STATE:
			result = AUDIO_IO_ERROR_INVALID_OPERATION;
			LOGE("[%s] (0x%08x) : Not recording started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
			break;
		default:
			result = __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
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

	if (handle->is_async) {
		LOGE ("Not supported in async mode");
		return AUDIO_IO_ERROR_INVALID_OPERATION;
	}

	ret = mm_sound_pcm_capture_ignore_session(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_capture_ignore_session() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_in_set_stream_cb(audio_in_h input, audio_in_stream_cb callback, void* userdata)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(callback);
	return audio_in_set_callback_private(input, callback, userdata);
}

int audio_in_unset_stream_cb(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	return audio_in_set_callback_private(input, NULL, NULL);
}

int audio_in_peek(audio_in_h input, const void **buffer, unsigned int *length)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	AUDIO_IO_NULL_ARG_CHECK(buffer);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = 0;
	int result = 0;

#ifdef _AUDIO_IO_DEBUG_TIMING_
	LOGE("handle->is_async : %d", handle->is_async);
#endif
	if (!handle->is_async) {
		LOGE ("audio_in_peek doesn't operate in poll mode!!!, use audio_in_read instead");
		return AUDIO_IO_ERROR_INVALID_OPERATION;
	}

#ifdef _AUDIO_IO_DEBUG_TIMING_
	LOGE("before mm_sound_pcm_capture_peek(handle[%p], buffer[%p], length[%d])", handle->mm_handle, buffer, length);
#endif
	ret = mm_sound_pcm_capture_peek(handle->mm_handle, buffer, length);
#ifdef _AUDIO_IO_DEBUG_TIMING_
	LOGE("after mm_sound_pcm_capture_peek() ret[%d]", ret);
#endif
	switch(ret)
	{
	case MM_ERROR_SOUND_INVALID_STATE:
		result = AUDIO_IO_ERROR_INVALID_OPERATION;
		LOGE("[%s] (0x%08x) : Not recording started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
		break;
	default:
		result = __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		break;
	}
	return result;
}

int audio_in_drop(audio_in_h input)
{
	AUDIO_IO_NULL_ARG_CHECK(input);
	audio_in_s *handle = (audio_in_s *) input;
	int ret = 0;
	int result = 0;

	if (!handle->is_async) {
		LOGE ("audio_in_drop doesn't operate in poll mode!!!, use audio_in_read instead");
		return AUDIO_IO_ERROR_INVALID_OPERATION;
	}

	ret = mm_sound_pcm_capture_drop(handle->mm_handle);
	if (ret == MM_ERROR_NONE) {
		return ret;
	}

	switch(ret)
	{
	case MM_ERROR_SOUND_INVALID_STATE:
		result = AUDIO_IO_ERROR_INVALID_OPERATION;
		LOGE("[%s] (0x%08x) : Not recording started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
		break;
	default:
		result = __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
		break;
	}
	return result;
}

/* Audio Out */
int audio_out_create(int sample_rate, audio_channel_e channel, audio_sample_type_e type, sound_type_e sound_type,  audio_out_h* output)
{
	return audio_out_create_private(sample_rate, channel, type, sound_type, output);
}

int audio_out_destroy(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_play_close_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_play_close(handle->mm_handle);
	}
	if (ret != MM_ERROR_NONE) {
		free(handle);
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}
	free(handle);

	LOGI("[%s] mm_sound_pcm_play_close() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_prepare(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_play_start_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_play_start(handle->mm_handle);
	}

	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_play_start() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_unprepare(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	audio_out_s *handle = (audio_out_s *) output;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_play_stop_async(handle->mm_handle);
	} else {
		ret = mm_sound_pcm_play_stop(handle->mm_handle);
	}

	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}

	LOGI("[%s] mm_sound_pcm_play_stop() success",__FUNCTION__);
	return AUDIO_IO_ERROR_NONE;
}

int audio_out_write(audio_out_h output, void* buffer, unsigned int length)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(buffer);
	audio_out_s *handle = (audio_out_s *) output;
	int ret = MM_ERROR_NONE;

	if (handle->is_async) {
		ret = mm_sound_pcm_play_write_async(handle->mm_handle, (void*) buffer, length);
	} else {
		ret = mm_sound_pcm_play_write(handle->mm_handle, (void*) buffer, length);
	}

	if (ret > 0)
		return ret;

	switch(ret)
	{
		case MM_ERROR_SOUND_INVALID_STATE:
			ret = AUDIO_IO_ERROR_INVALID_OPERATION;
			LOGE("[%s] (0x%08x) : Not playing started yet.",(char*)__FUNCTION__, AUDIO_IO_ERROR_INVALID_OPERATION);
			break;
		default:
			ret = __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
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

	if (handle->is_async) {
		LOGE ("Not supported in async mode");
		return AUDIO_IO_ERROR_INVALID_OPERATION;
	}

	ret = mm_sound_pcm_play_ignore_session(handle->mm_handle);
	if (ret != MM_ERROR_NONE) {
		return __convert_audio_io_error_code(ret, (char*)__FUNCTION__);
	}
	LOGI("[%s] mm_sound_pcm_play_ignore_session() success",__FUNCTION__);

	return AUDIO_IO_ERROR_NONE;
}

int audio_out_set_stream_cb(audio_out_h output, audio_out_stream_cb callback, void* userdata)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	AUDIO_IO_NULL_ARG_CHECK(callback);
	return audio_out_set_callback_private(output, callback, userdata);
}

int audio_out_unset_stream_cb(audio_out_h output)
{
	AUDIO_IO_NULL_ARG_CHECK(output);
	return audio_out_set_callback_private(output, NULL, NULL);
}

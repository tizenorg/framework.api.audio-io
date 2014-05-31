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

#include <tet_api.h>
#include <media/audio_io.h>

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_media_audio_in_create_p(void);
static void utc_media_audio_in_create_n(void);
static void utc_media_audio_in_destroy_p(void);
static void utc_media_audio_in_destroy_n(void);
static void utc_media_audio_in_read_p(void);
static void utc_media_audio_in_read_n(void);
static void utc_media_audio_in_get_buffer_size_p(void);
static void utc_media_audio_in_get_buffer_size_n(void);
static void utc_media_audio_in_get_sample_rate_p(void);
static void utc_media_audio_in_get_sample_rate_n(void);
static void utc_media_audio_in_get_channel_p(void);
static void utc_media_audio_in_get_channel_n(void);
static void utc_media_audio_in_get_sample_type_p(void);
static void utc_media_audio_in_get_sample_type_n(void);


struct tet_testlist tet_testlist[] = {
	{ utc_media_audio_in_create_p , POSITIVE_TC_IDX },
	{ utc_media_audio_in_create_n , NEGATIVE_TC_IDX },
	{ utc_media_audio_in_destroy_p, POSITIVE_TC_IDX },
	{ utc_media_audio_in_destroy_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_in_read_p, POSITIVE_TC_IDX },
	{ utc_media_audio_in_read_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_in_get_buffer_size_p, POSITIVE_TC_IDX },
	{ utc_media_audio_in_get_buffer_size_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_in_get_sample_rate_p, POSITIVE_TC_IDX },
	{ utc_media_audio_in_get_sample_rate_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_in_get_channel_p , POSITIVE_TC_IDX },
	{ utc_media_audio_in_get_channel_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_in_get_sample_type_p, POSITIVE_TC_IDX },
	{ utc_media_audio_in_get_sample_type_n, NEGATIVE_TC_IDX },
	{ NULL, 0 },
};

static void startup(void)
{
	/* start of TC */
}

static void cleanup(void)
{
	/* end of TC */
}

static void utc_media_audio_in_create_p(void)
{
	char* api_name = "audio_in_create";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		audio_in_destroy(input);
		dts_pass(api_name);
    }
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_create_n(void)
{
	char* api_name = "audio_in_create";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(48100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) != AUDIO_IO_ERROR_NONE)
    {
		dts_pass(api_name);
    }
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_destroy_p(void)
{
	char* api_name = "audio_in_destroy";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_in_destroy(input)) == AUDIO_IO_ERROR_NONE)
		{
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_destroy_n(void)
{
	char* api_name = "audio_in_destroy";
	int ret;
	if ((ret = audio_in_destroy(NULL)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_read_p(void)
{
	char* api_name = "audio_in_read";
	int ret, size;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE)
    {
		audio_in_prepare(input);
		if ((ret = audio_in_get_buffer_size(input, &size)) == AUDIO_IO_ERROR_NONE)
		{
			char *buffer = NULL;
			buffer = alloca(size);
			if ((ret = audio_in_read(input, (void*)buffer, size)) > AUDIO_IO_ERROR_NONE)
			{
				audio_in_destroy(input);
				dts_pass(api_name);
			}
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_read_n(void)
{
	char* api_name = "audio_in_read";
	int ret, size;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_in_get_buffer_size(input, &size)) == AUDIO_IO_ERROR_NONE)
		{
			char *buffer = NULL;
			buffer = alloca(size);
			if ((ret = audio_in_read(input, (void*)buffer, 0)) == AUDIO_IO_ERROR_NONE)
			{
				audio_in_destroy(input);
				dts_pass(api_name);
			}
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_buffer_size_p(void)
{
	char* api_name = "audio_in_get_buffer_size";
	int ret, size;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_in_get_buffer_size(input, &size)) == AUDIO_IO_ERROR_NONE)
		{
			audio_in_destroy(input);
			dts_pass(api_name);
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_buffer_size_n(void)
{
	char* api_name = "audio_in_get_buffer_size";
	int ret, size;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_in_get_buffer_size(input, NULL)) != AUDIO_IO_ERROR_NONE)
		{
			audio_in_destroy(input);
			dts_pass(api_name);
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_sample_rate_p(void)
{
	char* api_name = "audio_in_get_sample_rate";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		int sample_rate;
		if ((ret = audio_in_get_sample_rate(input, &sample_rate)) == AUDIO_IO_ERROR_NONE)
		{
			audio_in_destroy(input);
			dts_pass(api_name);
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_sample_rate_n(void)
{
	char* api_name = "audio_in_get_sample_rate";
	int ret, sample_rate;
    audio_in_h input = NULL;
	if ((ret = audio_in_get_sample_rate(input, &sample_rate)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_channel_p(void)
{
	char* api_name = "audio_in_get_channel";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		audio_channel_e channel;
		if ((ret = audio_in_get_channel(input, &channel)) == AUDIO_IO_ERROR_NONE)
		{
			audio_in_destroy(input);
			dts_pass(api_name);
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_channel_n(void)
{
	char* api_name = "audio_in_get_channel";
	int ret;
	audio_channel_e channel;
    audio_in_h input = NULL;
	if ((ret = audio_in_get_channel(input, &channel)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_sample_type_p(void)
{
	char* api_name = "audio_in_get_sample_type";
	int ret;
    audio_in_h input;
    if ((ret = audio_in_create(44100, AUDIO_CHANNEL_MONO ,AUDIO_SAMPLE_TYPE_S16_LE , &input)) == AUDIO_IO_ERROR_NONE)
    {
		audio_sample_type_e type;
		if ((ret = audio_in_get_sample_type(input, &type)) == AUDIO_IO_ERROR_NONE)
		{
			audio_in_destroy(input);
			dts_pass(api_name);
		}
		audio_in_destroy(input);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_in_get_sample_type_n(void)
{
	char* api_name = "audio_in_get_sample_type";
	int ret;
	audio_sample_type_e type;
    audio_in_h input = NULL;
	if ((ret = audio_in_get_sample_type(input, &type)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

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

#define AUDIO_FILE "/mnt/nfs/workspace/capi/media/audio-io/TC/media_samsung.mp3"

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_media_audio_out_create_p(void);
static void utc_media_audio_out_create_n(void);
static void utc_media_audio_out_destroy_p(void);
static void utc_media_audio_out_destroy_n(void);
static void utc_media_audio_out_write_p(void);
static void utc_media_audio_out_write_n(void);
static void utc_media_audio_out_get_buffer_size_p(void);
static void utc_media_audio_out_get_buffer_size_n(void);
static void utc_media_audio_out_get_sample_rate_p(void);
static void utc_media_audio_out_get_sample_rate_n(void);
static void utc_media_audio_out_get_channel_p(void);
static void utc_media_audio_out_get_channel_n(void);
static void utc_media_audio_out_get_sample_type_p(void);
static void utc_media_audio_out_get_sample_type_n(void);
static void utc_media_audio_out_get_sound_type_p(void);
static void utc_media_audio_out_get_sound_type_n(void);


struct tet_testlist tet_testlist[] = {
	{ utc_media_audio_out_create_p , POSITIVE_TC_IDX },
	{ utc_media_audio_out_create_n , NEGATIVE_TC_IDX },
	{ utc_media_audio_out_destroy_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_destroy_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_write_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_write_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_get_buffer_size_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_get_buffer_size_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_get_sample_rate_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_get_sample_rate_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_get_channel_p , POSITIVE_TC_IDX },
	{ utc_media_audio_out_get_channel_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_get_sample_type_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_get_sample_type_n, NEGATIVE_TC_IDX },
	{ utc_media_audio_out_get_sound_type_p, POSITIVE_TC_IDX },
	{ utc_media_audio_out_get_sound_type_n, NEGATIVE_TC_IDX },
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

static void utc_media_audio_out_create_p(void)
{
	char* api_name = "audio_out_create";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM, &output)) == AUDIO_IO_ERROR_NONE)
    {
		audio_out_destroy(output);
		dts_pass(api_name);
    }
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_create_n(void)
{
	char* api_name = "audio_out_create";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(48100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_SYSTEM , &output)) != AUDIO_IO_ERROR_NONE)
    {
		dts_pass(api_name);
    }
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_destroy_p(void)
{
	char* api_name = "audio_out_destroy";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_out_destroy(output)) == AUDIO_IO_ERROR_NONE)
		{
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_destroy_n(void)
{
	char* api_name = "audio_out_destroy";
	int ret;
	if ((ret = audio_out_destroy(NULL)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_write_p(void)
{
	char* api_name = "audio_out_write";
	int ret, size;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_out_get_buffer_size(output, &size)) == AUDIO_IO_ERROR_NONE)
		{
			FILE *fp = NULL;
			fp = fopen(AUDIO_FILE,"r");
			if(fp != NULL)
			{
				int read;
				char *buffer = NULL;
				buffer = alloca(size);
				if((read = fread(buffer, size, sizeof(char), fp)) > 0 )
				{
					audio_out_prepare(output);
					if ((ret = audio_out_write(output, (void*)buffer, read)) > AUDIO_IO_ERROR_NONE)
					{
						fclose(fp);
						audio_out_destroy(output);
						dts_pass(api_name);
					}
				}
				else
				{
					ret = AUDIO_IO_ERROR_INVALID_BUFFER;
				}
				fclose(fp);
			}
			else
			{
				ret = AUDIO_IO_ERROR_INVALID_BUFFER ;
			}
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_write_n(void)
{
	char* api_name = "audio_out_write";
	int ret, size;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_out_get_buffer_size(output, &size)) == AUDIO_IO_ERROR_NONE)
		{
			FILE *fp = NULL;
			fp = fopen(AUDIO_FILE,"r");
			if(fp != NULL)
			{
				char *buffer = NULL;
				int read;
				audio_out_prepare(output);
				if ((ret = audio_out_write(output, (void*)buffer, read)) != AUDIO_IO_ERROR_NONE)
				{
					fclose(fp);
					audio_out_destroy(output);
					dts_pass(api_name);
				}
				fclose(fp);
			}
			else
			{
				ret = AUDIO_IO_ERROR_INVALID_BUFFER ;
			}
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_buffer_size_p(void)
{
	char* api_name = "audio_out_get_buffer_size";
	int ret, size;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM, &output)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_out_get_buffer_size(output, &size)) == AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_buffer_size_n(void)
{
	char* api_name = "audio_out_get_buffer_size";
	int ret, size;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM, &output)) == AUDIO_IO_ERROR_NONE)
    {
		if ((ret = audio_out_get_buffer_size(output, NULL)) != AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sample_rate_p(void)
{
	char* api_name = "audio_out_get_sample_rate";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		int sample_rate;
		if ((ret = audio_out_get_sample_rate(output, &sample_rate)) == AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sample_rate_n(void)
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

static void utc_media_audio_out_get_channel_p(void)
{
	char* api_name = "audio_out_get_channel";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		audio_channel_e channel;
		if ((ret = audio_out_get_channel(output, &channel)) == AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_channel_n(void)
{
	char* api_name = "audio_out_get_channel";
	int ret;
	audio_channel_e channel;
    audio_out_h output = NULL;
	if ((ret = audio_out_get_channel(output, &channel)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sample_type_p(void)
{
	char* api_name = "audio_out_get_sample_type";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		audio_sample_type_e type;
		if ((ret = audio_out_get_sample_type(output, &type)) == AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sample_type_n(void)
{
	char* api_name = "audio_out_get_sample_type";
	int ret;
	audio_sample_type_e type;
    audio_out_h output = NULL;
	if ((ret = audio_out_get_sample_type(output, &type)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sound_type_p(void)
{
	char* api_name = "audio_out_get_sound_type";
	int ret;
    audio_out_h output;
    if ((ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE , SOUND_TYPE_SYSTEM , &output)) == AUDIO_IO_ERROR_NONE)
    {
		sound_type_e type;
		if ((ret = audio_out_get_sound_type(output, &type)) == AUDIO_IO_ERROR_NONE)
		{
			audio_out_destroy(output);
			dts_pass(api_name);
		}
		audio_out_destroy(output);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_media_audio_out_get_sound_type_n(void)
{
	char* api_name = "audio_out_get_sound_type";
	int ret;
	audio_sample_type_e type;
    audio_out_h output = NULL;
	if ((ret = audio_out_get_sample_type(output, &type)) != AUDIO_IO_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

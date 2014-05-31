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
#include <audio_io.h>

static int ch_table[3] = { 0, AUDIO_CHANNEL_MONO, AUDIO_CHANNEL_STEREO };

void play_file(char *file, int length, int ch)
{
	audio_out_h output;
	FILE* fp = fopen (file, "r");

	char * buf = malloc (length);

	printf ("start to play [%s][%d][%d]\n", file, length, ch);
	audio_out_create(44100, ch_table[ch] ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_MEDIA, &output);
	if (fread (buf, 1, length, fp) != length) {
		printf ("error!!!!\n");
	}

	audio_out_prepare(output);
	audio_out_write(output, buf, length);
	audio_out_unprepare(output);

	audio_out_destroy (output);

	fclose (fp);

	printf ("play done\n");
}

#define DUMP_FILE "/root/test.raw"

int audio_io_test(int length, int num, int ch)
{
	int ret, size, i;
	audio_in_h input;
	if ((ret = audio_in_create(44100, ch_table[ch] ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE) {
		ret = audio_in_ignore_session(input);
		if (ret != 0) {
			printf ("ERROR, set session mix\n");
			audio_in_destroy(input);
			return 0;
		}

		ret = audio_in_prepare(input);
		if (ret != 0) {
			printf ("ERROR, prepare\n");
			audio_in_destroy(input);
			return 0;
		}

		FILE* fp = fopen (DUMP_FILE, "wb+");

		if ((ret = audio_in_get_buffer_size(input, &size)) == AUDIO_IO_ERROR_NONE) {
			size = length;
			char *buffer = alloca(size);

			for (i=0; i<num; i++) {
				printf ("### loop = %d ============== \n", i);
				if ((ret = audio_in_read(input, (void*)buffer, size)) > AUDIO_IO_ERROR_NONE) {
					fwrite (buffer, size, sizeof(char), fp);
					printf ("PASS, size=%d, ret=%d\n", size, ret);
				}
				else {
					printf ("FAIL, size=%d, ret=%d\n", size, ret);
				}
			}
		}

		fclose (fp);

		audio_in_destroy(input);
	}

	play_file (DUMP_FILE, length*num, ch);

	return 1;
}

int audio_io_test_ex()
{
	int ret, size;
	audio_in_h input;
	if ((ret = audio_in_create_ex(44100, AUDIO_CHANNEL_STEREO , AUDIO_SAMPLE_TYPE_S16_LE, &input, 1)) == AUDIO_IO_ERROR_NONE) {
		ret = audio_in_ignore_session(input);
		if (ret != 0) {
			printf ("ERROR, set session mix\n");
			audio_in_destroy(input);
			return 0;
		}

		ret = audio_in_prepare(input);
		if (ret != 0) {
			printf ("ERROR, prepare\n");
			audio_in_destroy(input);
			return 0;
		}

		if ((ret = audio_in_get_buffer_size(input, &size)) == AUDIO_IO_ERROR_NONE) {
			size = 500000;
			char *buffer = alloca(size);
			if ((ret = audio_in_read(input, (void*)buffer, size)) > AUDIO_IO_ERROR_NONE) {
				FILE* fp = fopen ("/root/test.raw", "wb+");
				fwrite (buffer, size, sizeof(char), fp);
				fclose (fp);
				printf ("PASS, size=%d, ret=%d\n", size, ret);
			}
			else {
				printf ("FAIL, size=%d, ret=%d\n", size, ret);
			}
		}
		audio_in_destroy(input);
	}

	return 1;
}

int main(int argc, char ** argv)
{
	if((argc == 2)&&(!strcmp(argv[1], "mirroring"))) {
		audio_io_test_ex();
	} else if (argc == 4) {
		printf ("run with [%s][%s][%s]\n", argv[1],argv[2],argv[3]);
#if 0
		audio_io_test(atoi (argv[1]), atoi (argv[2]), atoi(argv[3]));
#endif
	} else {
		printf ("1. usage : audio_io_test [length to read] [number of iteration] [channels]\n");
		printf ("2. usage : audio_io_test mirroring\n");
	}
	return 0;
}


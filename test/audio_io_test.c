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
#include <unistd.h>
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


void play_file_sample(char *file, int frequency, int ch, int type)
{
	audio_out_h output;
	int file_size = 0;
	int read_bytes = 0;
	int buffer_size = 0;
	char * buf = NULL;
	FILE* fp = fopen (file, "r");
	/*Get the size*/
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	printf ("start to play [%s] of size [%d] with [%d][%d][%d]\n", file, file_size, frequency, ch, type);
	if (type) {
		audio_out_create(frequency, ch_table[ch] ,AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_MEDIA, &output);
	}
	else {
		audio_out_create(frequency, ch_table[ch] ,AUDIO_SAMPLE_TYPE_U8, SOUND_TYPE_MEDIA, &output);
	}
	audio_out_get_buffer_size(output, &buffer_size);

	buf = (char *) malloc(buffer_size);
	audio_out_prepare(output);

	while (file_size > 0) {
		read_bytes = fread (buf, 1, buffer_size, fp);
		printf ("Read %d Requested - %d\n", read_bytes, buffer_size);
		audio_out_write(output, buf, read_bytes);
		file_size = file_size - read_bytes;
	}

	audio_out_unprepare(output);
	audio_out_destroy (output);

	free(buf);
	fclose (fp);
	printf ("play done\n");
}

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


int audio_io_loopback_in_test()
{
    int ret, size;
    audio_in_h input;
    FILE* fp = fopen ("/tmp/dump_test.raw", "wb+");

    if(fp == NULL) {
        printf("open failed \n");
        return 0;
    }

    if ((ret = audio_in_create(16000, AUDIO_CHANNEL_MONO , AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE) {
        ret = audio_in_ignore_session(input);
        if (ret != 0) {
            printf ("ERROR, set session mix\n");
            goto exit;
        }

        ret = audio_in_prepare(input);
        if (ret != 0) {
            printf ("ERROR, prepare\n");
            goto exit;
        }

        ret = audio_in_get_buffer_size(input, &size);
        if(ret != AUDIO_IO_ERROR_NONE) {
            printf ("audio_in_get_buffer_size failed.\n");
            goto exit;
        }

        while(1) {
            char *buffer = alloca(size);
            if ((ret = audio_in_read(input, (void*)buffer, size)) > AUDIO_IO_ERROR_NONE) {
                fwrite (buffer, size, sizeof(char), fp);
                printf ("PASS, size=%d, ret=%d\n", size, ret);
            }
            else {
                printf ("FAIL, size=%d, ret=%d\n", size, ret);
            }
        }
    }

exit:
    audio_in_destroy(input);

    fclose (fp);

    return ret;

}

int audio_io_loopback_test()
{
    int ret, size;
    audio_in_h input;
    audio_out_h output;
    char *buffer = NULL;

    ret = audio_in_create(16000, AUDIO_CHANNEL_MONO , AUDIO_SAMPLE_TYPE_S16_LE, &input);
    if(ret != AUDIO_IO_ERROR_NONE) {
        printf ("audio_in_create_ex failed. \n");
        return 0;
    }

    ret = audio_out_create(16000, AUDIO_CHANNEL_MONO , AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_CALL, &output);
    if(ret != AUDIO_IO_ERROR_NONE) {
        printf ("audio_out_create failed. \n");
        return 0;
    }

    ret = audio_in_prepare(input);
    if (ret != 0) {
        printf ("audio_in_prepare failed.\n");
        audio_in_destroy(input);
        return 0;
    } else {
        ret = audio_in_get_buffer_size(input, &size);
        if(ret != AUDIO_IO_ERROR_NONE) {
            printf ("audio_in_get_buffer_size failed.\n");
            return 0;
        }
        else {
            printf("size(%d)\n", size);
            buffer = alloca(size);
        }
    }

    ret = audio_out_prepare(output);
    if (ret != 0) {
        printf ("audio_out_prepare failed.\n");
        audio_out_destroy(output);
        return 0;
    }

    if(buffer == NULL) {
        printf("buffer is null\n");
        return 0;
    }

    while(1) {
        ret = audio_in_read(input, (void*)buffer, size);
        if(ret > AUDIO_IO_ERROR_NONE) {
            ret = audio_out_write(output, buffer, size);
            if(ret > AUDIO_IO_ERROR_NONE) {
                printf("audio read/write success. buffer(%p), size(%d)\n", buffer, size);
            } else {
                printf("audio read success, write failed. buffer(%p), size(%d)\n", buffer, size);
            }
        } else
            printf("audio read/write failed. buffer(%p), size(%d)\n", buffer, size);
    }

}

audio_in_h input;
audio_out_h output;

FILE* fp_r = NULL;
FILE* fp_w = NULL;

static void _audio_io_stream_read_cb (audio_in_h handle, size_t nbytes, void *userdata)
{
	const void * buffer = NULL;

//	printf("_audio_io_stream_read_cb : handle=%p, nbytes=%d, userdata=%p\n", handle, nbytes, userdata);

	if (nbytes > 0) {
		audio_in_peek (handle, &buffer, &nbytes);
		if (fp_w) {
			fwrite(buffer, sizeof(char), nbytes, fp_w);
		}
		audio_in_drop (handle);
	}
}

static void _audio_io_stream_write_cb (audio_out_h handle, size_t nbytes, void *userdata)
{
	char* buffer = NULL;
	int ret = 0;

//	printf("_audio_io_stream_write_cb : handle=%p, nbytes=%d, userdata=%p\n", handle, nbytes, userdata);

	if (nbytes > 0) {
		buffer = malloc (nbytes);
		memset (buffer, 0, nbytes);

		ret = fread (buffer, 1, nbytes, fp_r);
		if (ret != nbytes) {
			printf ("Error!!!! %d/%d", ret, nbytes);
		}

		ret = audio_out_write(handle, buffer, nbytes);
		if(ret > AUDIO_IO_ERROR_NONE) {
//			printf("audio write success. buffer(%p), nbytes(%d)\n", buffer, nbytes);
		}

		free (buffer);
	}
}

int audio_io_async_test(int mode)
{
	int ret, size;

	char *buffer = NULL;
	int i=0;

	int read_mode = (mode & 0x02);
	int write_mode = (mode & 0x01);

	if (read_mode) {
		printf ("audio_in_create\n");
		ret = audio_in_create(44100, AUDIO_CHANNEL_STEREO , AUDIO_SAMPLE_TYPE_S16_LE, &input);
		if(ret != AUDIO_IO_ERROR_NONE) {
			printf ("audio_in_create_ex failed. \n");
			return 0;
		}
		printf ("audio_in_create success!!! [%p]\n", input);

		ret = audio_in_set_stream_cb(input, _audio_io_stream_read_cb, NULL);
		if(ret != AUDIO_IO_ERROR_NONE) {
			printf ("audio_in_set_stream_cb failed. \n");
			return 0;
		}
		printf ("audio_in_set_stream_cb success!!! [%p]\n", input);

		fp_w = fopen( "/tmp/pcm_w.raw", "w");
	}

	if (write_mode) {
		printf ("before audio_out_create\n");
		getchar();
		printf ("audio_out_create\n");
		ret = audio_out_create(44100, AUDIO_CHANNEL_STEREO , AUDIO_SAMPLE_TYPE_S16_LE, SOUND_TYPE_MEDIA, &output);
		if(ret != AUDIO_IO_ERROR_NONE) {
			printf ("audio_out_create failed. \n");
			return 0;
		}
		printf ("audio_out_create success!!! [%p]\n", output);

		ret = audio_out_set_stream_cb(output, _audio_io_stream_write_cb, NULL);
		if(ret != AUDIO_IO_ERROR_NONE) {
			printf ("audio_out_set_stream_cb failed. \n");
			return 0;
		}
		printf ("audio_out_set_stream_cb success!!! [%p]\n", input);

		fp_r = fopen( "/tmp/pcm.raw", "r");
	}

	if (read_mode) {
		printf ("before audio_in_prepare\n");
		getchar();
		printf ("audio_in_prepare\n");
		ret = audio_in_prepare(input);
		if (ret != 0) {
			printf ("audio_in_prepare failed.\n");
			audio_in_destroy(input);
			return 0;
		} else {
			ret = audio_in_get_buffer_size(input, &size);
			if(ret != AUDIO_IO_ERROR_NONE) {
				printf ("audio_in_get_buffer_size failed.\n");
				return 0;
			}
			else {
				printf("size(%d)\n", size);
				buffer = alloca(size);
			}
		}

		if(buffer == NULL) {
			printf("buffer is null\n");
			return 0;
		}
	}

	if (write_mode) {
		printf ("before audio_out_prepare\n");
		getchar();
		printf ("audio_out_prepare\n");
		ret = audio_out_prepare(output);
		if (ret != 0) {
			printf ("audio_out_prepare failed.\n");
			audio_out_destroy(output);
			return 0;
		}
	}

	//getchar();

	printf ("loop start\n");
	for (i=0; i<10; i++) {
		printf ("-------- %d -------\n",i);
		usleep (1000000);
	}

	//getchar();

	if (read_mode) {
		printf ("audio_in_unprepare\n");
		audio_in_unprepare(input);
		printf ("audio_in_destroy\n");
		audio_in_destroy(input);

		fclose(fp_w);
		fp_w = NULL;
	}

	getchar();

	if (write_mode) {
		printf ("audio_out_unprepare\n");
		audio_out_unprepare(output);
		printf ("audio_out_destroy\n");
		audio_out_destroy(output);

		fclose(fp_r);
		fp_r = NULL;
	}

	return 0;
}

int main(int argc, char ** argv)
{
	if ( argc == 2 && !strcmp(argv[1],"call-forwarding-loop")) {
		audio_io_loopback_test();
	} else if ( argc == 2 && !strcmp(argv[1],"call-forwarding-in")) {
		audio_io_loopback_in_test();
	} else if ( argc == 3 && !strcmp(argv[1],"async")) {
		audio_io_async_test(atoi(argv[2]));
	} else if (argc == 4) {
		printf ("run with [%s][%s][%s]\n", argv[1],argv[2],argv[3]);
#if 0
		audio_io_test(atoi (argv[1]), atoi (argv[2]), atoi(argv[3]));
#endif
	} else if (argc == 6) {
		play_file_sample(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	} else {
		printf ("1. usage : audio_io_test [length to read] [number of iteration] [channels]\n");
		printf ("2. usage : audio_io_test mirroring\n");
		printf ("3. Uasge : audio_io_test play <filename> <sample rate> <channels> <type(0:U8)>");
	}
	return 0;
}


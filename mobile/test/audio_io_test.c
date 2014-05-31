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

int audio_io_test()
{
	int ret, size;
	audio_in_h input;
	if ((ret = audio_in_create(44100, AUDIO_CHANNEL_STEREO ,AUDIO_SAMPLE_TYPE_S16_LE, &input)) == AUDIO_IO_ERROR_NONE) {
		ret = audio_in_ignore_session(input);
		if (ret != 0) {
			printf ("ERROR, set session mix\n");
			audio_in_destroy(input);
			return 0;
		}

		audio_in_prepare(input);
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
	audio_io_test();
	return 0;
}


#include <stdio.h>
#include <stdint.h>

#define WRITE(x) if (fwrite(&x, sizeof(x), 1, wav) != 1) { perror("Error writing to wav file"); goto error; }
#define SEEK(x) if (fseek(wav, x, SEEK_SET)) { perror("Error seeking in wav file"); goto error; }

int main(int argc, char * argv[])
{
	// banner
	if (argc != 2) {
		printf("Usage: %s output.wav\n", argv[0]);
		return 1;
	}

	// attempt to reopen stdin in binary mode
	if (! freopen(NULL, "rb", stdin)) {
		perror("Failed to reopen stdin as binary");
		return 1;
	}

	// open wav file for writing
	FILE * wav = fopen(argv[1], "wb");

	if (! wav) {
		perror("Failed to open output file");
		return 1;
	}

	// dump wav header
	//                          R     I     F     F     filesize    W     A     V     E
	const uint8_t header[] = { 0x52, 0x49, 0x46, 0x46, 36, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,
	//                          f     m     t           chunksize   PCM   Chan  samplerate        byterate             align bitdepth
					0x66, 0x6d, 0x74, 0x20, 16, 0, 0, 0, 1, 0, 2, 0, 0x44, 0xAC, 0, 0, 0x10, 0xB1, 0x02, 0, 4, 0, 16, 0,
	//                          d     a     t     a     chunksize
					0x64, 0x61, 0x74, 0x61, 0, 0, 0, 0
	};
	WRITE(header);

	// flags
	int start = 0;
	int done = 0;
	// counters
	unsigned long samples = 0;
	unsigned long trim_begin = 0;
	unsigned long trim_end = 0;

	while (! done) {
		// read one sample from stdin
		uint32_t sample;

		if (fread(&sample, sizeof(uint32_t), 1, stdin) == 1) {
			if (! start) {
				// trimming beginning values
				if (sample != 0) {
					start = 1;
					samples ++;
					WRITE(sample)
				} else
					trim_begin ++;
			} else {
				// checking for trailing values
				if (sample != 0) {
					// write any collected zero samples
					samples += trim_end;

					while (trim_end) {
						const uint32_t zero = 0;
						WRITE(zero)
						trim_end --;
					}

					samples ++;
					WRITE(sample)
				} else
					trim_end ++;
			}
		} else {
			// short read from stdin
			if (! feof(stdin))
				perror("Error reading from stdin");

			done = 1;
		}
	}

	// seek to wav header positions and rewrite sizes as needed
	SEEK(4);
	uint32_t size = samples * 4 + 36;
	WRITE(size)
	SEEK(40);
	size = samples * 4;
	WRITE(size)

	// all done!
	fclose(wav);

	printf("Samples: %lu\tTrimmed start: %lu\tTrimmed end: %lu\tTrimmed total: %lu\n", samples, trim_begin, trim_end, trim_begin + trim_end);
	return 0;

error:
	fclose(wav);
	return 1;
}

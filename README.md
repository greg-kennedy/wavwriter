# wavwriter

this is a tool to wrap 44100hz s16le stereo raw audio (from stdin) in a wave wrapper and save it to a file.

Usage: `./wavwriter output.wav`

That's all it does, but it ALSO has exactly one trick: any leading or trailing silence (all-0 sample in both channels) is trimmed on output.

Example ffmpeg invocation:
`ffmpeg -loglevel error -hide_banner -nostats -bitexact -i $INPUT_FILE -vn -ac 2 -ar 44100 -acodec pcm_s16le -map_metadata -1 -flags +bitexact -fflags +bitexact -f s16le pipe:1 | wavwriter $OUTPUT_FILE`
to transcode any input file to a .wav with no leading nor trailing silence.

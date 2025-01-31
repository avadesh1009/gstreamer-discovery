#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <vector>
#include <string>

// Structures to hold media stream information
struct AudioInfo {
	std::string codec;
	int channels;
	int sample_rate;
	int depth;
	int bitrate;
	int max_bitrate;
};

struct VideoInfo {
	std::string codec;
	int width;
	int height;
	int depth;
	float frame_rate;
	float pixel_aspect_ratio;
	bool is_interlaced;
	int bitrate;
	int max_bitrate;
};

struct SubtitleInfo {
	std::string codec;
	std::string language_name;
};

struct StreamInfo {
	std::vector<AudioInfo> audio_streams;
	std::vector<VideoInfo> video_streams;
	std::vector<SubtitleInfo> subtitle_streams;
};

#endif // STRUCT_H
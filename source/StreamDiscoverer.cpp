#include "stdafx.h"
#include "StreamDiscoverer.h"

StreamInfo StreamDiscoverer::stream_info;

std::string StreamDiscoverer::extractAfterLastSlash(const std::string& input) {
	size_t pos = input.find_last_of('/');
	return (pos != std::string::npos) ? input.substr(pos + 1) : input;
}

std::string StreamDiscoverer::getCodecFromStreamInfo(GstDiscovererStreamInfo *stream_info_data) {
	GstCaps* caps = gst_discoverer_stream_info_get_caps(stream_info_data);
	GstStructure *structure = gst_caps_get_structure(caps, 0);
	return extractAfterLastSlash(std::string(gst_structure_get_name(structure)));
}

void StreamDiscoverer::ProcessStreams(GstDiscovererInfo *info) {
	GList* stream_list = gst_discoverer_info_get_stream_list(info);
	for (GList *item = stream_list; item != NULL; item = item->next) {
		GstDiscovererStreamInfo *stream_info_data = GST_DISCOVERER_STREAM_INFO(item->data);
		const gchar *stream_type = gst_discoverer_stream_info_get_stream_type_nick(stream_info_data);

		if (g_strcmp0(stream_type, "video") == 0) {
			ProcessVideoStream(stream_info_data);
		}
		else if (g_strcmp0(stream_type, "audio") == 0) {
			ProcessAudioStream(stream_info_data);
		}
		else if (g_strcmp0(stream_type, "subtitle") == 0) {
			ProcessSubtitleStream(stream_info_data);
		}
	}
}

void StreamDiscoverer::ProcessVideoStream(GstDiscovererStreamInfo *stream_info_data) {
	GstDiscovererVideoInfo *video_info = GST_DISCOVERER_VIDEO_INFO(stream_info_data);

	VideoInfo video_data;
	video_data.codec = getCodecFromStreamInfo(stream_info_data);
	video_data.width = gst_discoverer_video_info_get_width(video_info);
	video_data.height = gst_discoverer_video_info_get_height(video_info);
	video_data.depth = gst_discoverer_video_info_get_depth(video_info);
	video_data.frame_rate = static_cast<float>(gst_discoverer_video_info_get_framerate_num(video_info)) /
		gst_discoverer_video_info_get_framerate_denom(video_info);
	video_data.pixel_aspect_ratio = static_cast<float>(gst_discoverer_video_info_get_par_num(video_info)) /
		gst_discoverer_video_info_get_par_denom(video_info);
	video_data.is_interlaced = gst_discoverer_video_info_is_interlaced(video_info);
	video_data.bitrate = gst_discoverer_video_info_get_bitrate(video_info);
	video_data.max_bitrate = gst_discoverer_video_info_get_max_bitrate(video_info);

	stream_info.video_streams.push_back(video_data);
}

void StreamDiscoverer::ProcessAudioStream(GstDiscovererStreamInfo *stream_info_data) {
	GstDiscovererAudioInfo *audio_info = GST_DISCOVERER_AUDIO_INFO(stream_info_data);

	AudioInfo audio_data;
	audio_data.codec = getCodecFromStreamInfo(stream_info_data);
	audio_data.channels = gst_discoverer_audio_info_get_channels(audio_info);
	audio_data.sample_rate = gst_discoverer_audio_info_get_sample_rate(audio_info);
	audio_data.depth = gst_discoverer_audio_info_get_depth(audio_info);
	audio_data.bitrate = gst_discoverer_audio_info_get_bitrate(audio_info);
	audio_data.max_bitrate = gst_discoverer_audio_info_get_max_bitrate(audio_info);

	stream_info.audio_streams.push_back(audio_data);
}

void StreamDiscoverer::ProcessSubtitleStream(GstDiscovererStreamInfo *stream_info_data) {
	GstDiscovererSubtitleInfo *subtitle_info = GST_DISCOVERER_SUBTITLE_INFO(stream_info_data);

	SubtitleInfo subtitle_data;
	subtitle_data.codec = getCodecFromStreamInfo(stream_info_data);
	subtitle_data.language_name = gst_discoverer_subtitle_info_get_language(subtitle_info);

	stream_info.subtitle_streams.push_back(subtitle_data);
}

void StreamDiscoverer::OutputStreamInfo() {
	std::cout << "Video Streams: " << std::endl;
	for (const auto &video : stream_info.video_streams) {
		std::cout << "  Codec: " << video.codec << ", Resolution: " << video.width << "x" << video.height
			<< ", Frame Rate: " << video.frame_rate << ", Bitrate: " << video.bitrate << std::endl;
	}

	std::cout << "Audio Streams: " << std::endl;
	for (const auto &audio : stream_info.audio_streams) {
		std::cout << "  Codec: " << audio.codec << ", Channels: " << audio.channels
			<< ", Sample Rate: " << audio.sample_rate << ", Bitrate: " << audio.bitrate << std::endl;
	}

	std::cout << "Subtitle Streams: " << std::endl;
	for (const auto &subtitle : stream_info.subtitle_streams) {
		std::cout << "  Codec: " << subtitle.codec << ", Language: " << subtitle.language_name << std::endl;
	}
}

const StreamInfo& StreamDiscoverer::getStreamInfo() {
	return stream_info;
}

bool StreamDiscoverer::DiscoverStream(const gchar *uri) {
	GError *error = NULL;
	GstDiscoverer *discoverer = gst_discoverer_new(5 * GST_SECOND, &error);

	if (error != NULL) {
		std::cerr << "Failed to create GstDiscoverer: " << error->message << std::endl;
		g_error_free(error);
		return false;
	}

	GstDiscovererInfo *info = gst_discoverer_discover_uri(discoverer, uri, &error);

	if (error != NULL) {
		std::cerr << "Failed to discover URI: " << error->message << std::endl;
		g_error_free(error);
		g_object_unref(discoverer);
		return false;
	}

	ProcessStreams(info);
	std::cout << "Stream discovery successful." << std::endl;

	g_object_unref(discoverer);
	g_object_unref(info);
	return true;
}
#include "stdafx.h"
#include "StreamDiscoverer.h"

int main(int argc, char *argv[]) {
	
	 gst_init(&argc, &argv);

	const gchar *uri = "rtsp://admin:admin12345@192.168.111.179:554/sub";  // Your RTSP URL
	//const gchar *uri = "rtsp://admin:admin@192.168.111.126/unicaststream/1";  // Your RTSP URL

	bool success = StreamDiscoverer::DiscoverStream(uri);

	if (success) {
		StreamDiscoverer::OutputStreamInfo();
		std::cout << "Stream discovery completed successfully." << std::endl;
	}
	else {
		std::cerr << "Stream discovery failed." << std::endl;
	}

	return 0;
}

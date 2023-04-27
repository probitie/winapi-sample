#include "../shared/stb_image.h"
#include "../shared/lbdll.h"
#include <iostream>

#define LB_IMG "./testimg.jpg" // in build directory

int main()
{
	int h{}, w{}, channels{};
	auto img = stbi_load(LB_IMG, &w, &h, &channels, 0);

	// test sending
	// TODO use gtest / or custom header testing
	// TODO in a thread and receive in the second thread
	//send_image("127.0.0.1", 8888, img, w, h, channels);
	
	//int r_h{}, r_w{}, r_channels{}; // r_ means received
	//auto recv_img = receive_image(8888, &r_w, &r_h, &r_channels);

	// test processing
	process_image(img, w, h, channels,
		[](auto& r, auto& g, auto& b)
		{
			r += 10;
			g += 20;
			b += 30;
		});

	// count black pixels
	const int blacks = count_colored_pixels(img, w, h, channels, 0, 0, 0);

	std::cout << blacks;
	return 0;
}
#include "../stb_image_lib/stb_image.h"
#include "../lbdll/lbdll.h"
#include <iostream>
#include <thread>




int main()
{
	int h{}, w{}, channels{};
	auto img = stbi_load(LB_IMG, &w, &h, &channels, 0);


	// test sending

	// run sending in separate thread but receive in current
	std::thread t1{ send_image, LB_HOST, LB_PORT, img, w, h, channels };

	int r_h{}, r_w{}, r_channels{}; // r_ means received
	auto recv_img = receive_image(LB_PORT, &r_w, &r_h, &r_channels);

	// wait until the thread closes
	// (even if it should've been)
	t1.join();

	LB_ASSERT(memcmp(img, recv_img, w*h*channels) == 0, "sending by localhost failed - resulting image is not equal to the origin");


	// test processing

	process_image(img, w, h, channels,
		[](auto& r, auto& g, auto& b)
		{
			r = 0; // as 'r' is 8-bit value it will go round after overflow
			g = 0; // exactly what's expected
			b = 0;
		});

	// count black pixels
	const int blacks = count_colored_pixels(img, w, h, channels, 0, 0, 0);

	// prevent memory leaks
	stbi_image_free(img);
	img = nullptr;

	std::cout << blacks;
	return 0;
}
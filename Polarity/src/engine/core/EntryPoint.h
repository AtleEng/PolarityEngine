#pragma once

#ifdef POLARITY_PLATFORM_WINDOWS

extern Polarity::Application* Polarity::CreateApplication();

int main(int argc, char** argv)
{
	LOG_INFO("PolarityEngine initializing...");

	auto app = Polarity::CreateApplication();
	app->Run();
	delete app;
}

#endif

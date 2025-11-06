#pragma once

#ifdef POLARITY_PLATFORM_WINDOWS

extern Polarity::Application* Polarity::CreateApplication();

int main(int argc, char** argv)
{
	LOG_EX("", textColorGreen, "PolarityEngine v1.0.0\n  Dev build\n");
	LOG_INFO("Engine initializing...");

	auto app = Polarity::CreateApplication();
	app->Run();
	delete app;
}

#endif

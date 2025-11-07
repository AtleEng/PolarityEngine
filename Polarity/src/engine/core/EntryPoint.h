#pragma once

#ifdef POLARITY_PLATFORM_WINDOWS

extern Polarity::Application* Polarity::CreateApplication();

int main(int argc, char** argv)
{
	LOG_EX("[PolarityEngine]", textColorOrange, "Dev build\n");

	auto app = Polarity::CreateApplication();
	app->Run();
	delete app;
}

#endif

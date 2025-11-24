#pragma once

#ifdef POLARITY_PLATFORM_WINDOWS

extern Polarity::Application* Polarity::CreateApplication();

int main(int argc, char** argv)
{
	POLARITY_PROFILE_BEGIN_SESSION("Startup", "profile/Startup.json");
	auto app = Polarity::CreateApplication();
	POLARITY_PROFILE_END_SESSION();

	POLARITY_PROFILE_BEGIN_SESSION("Runtime", "profile/Runtime.json");
	app->Run();
	POLARITY_PROFILE_END_SESSION();

	POLARITY_PROFILE_BEGIN_SESSION("Shutdown", "profile/Shutdown.json");
	delete app;
	POLARITY_PROFILE_END_SESSION();
}

#endif

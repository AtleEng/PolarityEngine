#pragma once

#ifdef ATLAS_PLATFORM_WINDOWS

extern Atlas::Application* Atlas::CreateApplication();

int main(int argc, char** argv)
{
	printf("Atlas: starting...\n");

	auto app = Atlas::CreateApplication();
	app->Run();
	delete app;
}

#endif // ATLAS_PLATFORM_WINDOWS

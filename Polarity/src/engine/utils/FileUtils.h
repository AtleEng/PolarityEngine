#pragma once

#include <sys/stat.h> //edit timestamp of files

namespace Polarity
{

	long long GetTimestamp(const char* file)
	{
		struct stat file_stat = {};
		stat(file, &file_stat);
		return file_stat.st_mtime;
	}
}
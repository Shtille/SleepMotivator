/*************************************************************************
*  Sleep Motivator
* ------------------------------------------------------------------------
*  Copyright (c) 2018 Vladimir Sviridov <v.shtille@gmail.com>
*
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
*
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
*
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
*
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
*
*  3. This notice may not be removed or altered from any source
*     distribution.
*
**************************************************************************/

#include "ini_file.h"

#include <cstddef>
#include <cstring>

namespace sm {

	IniFile::IniFile()
	: file_(nullptr)
	{

	}
	IniFile::~IniFile()
	{
		Close();
	}
	bool IniFile::OpenForWrite(const char* filename)
	{
		errno_t err = fopen_s(&file_, filename, "wt");
		return err == 0;
	}
	bool IniFile::OpenForRead(const char* filename)
	{
		errno_t err = fopen_s(&file_, filename, "rt");
		return err == 0;
	}
	void IniFile::Close()
	{
		if (file_)
		{
			fclose(file_);
			file_ = nullptr;
		}
	}
	bool IniFile::WriteSection(const char* section_name)
	{
		return fprintf(file_, "[%s]\n", section_name) == 1;
	}
	bool IniFile::WritePair(const char* key, const char* value)
	{
		return fprintf(file_, "%s = %s\n", key, value) == 2;
	}
	void IniFile::Read(IniFileReaderInterface * reader)
	{
		char buffer[256];
		while (!feof(file_) && ReadLine(buffer))
		{
			size_t len = strlen(buffer);
			if (len > 2 && buffer[0] == '[' && buffer[len-1] == ']')
			{
				// Section have been recognized
				buffer[len-1] = '\0';
				reader->OnSection(buffer + 1);
			}
			else
			{
				char * pos = strchr(buffer, '=');
				if (pos == nullptr)
				{
					// No entrance was found
					reader->OnPair(buffer, nullptr);
				}
				else
				{
					// We should determine correct lengths of lvalue and rvalue
					const char kSpace = ' ';
					// Remove backward spaces
					char * c = pos - 1;
					while ((c != buffer) && (*c == kSpace))
					{
						*c = '\0';
						--c;
					}
					// Remove forward spaces
					c = pos + 1;
					while ((*c != '\0') && (*c == kSpace))
					{
						++c;
					}
					reader->OnPair(buffer, c);
				}
			}
		}
	}
	bool IniFile::ReadLine(char text[256])
	{
		do
		{
			if (fgets(text, 255, file_) == nullptr)
				return false;
		}
		while (!feof(file_) && (text[0] == '\n' || text[0] == '/'));
		char& c = text[strlen(text) - 1];
		if (c == '\n') c = '\0';
		text[255] = '\0';
		return true;
	}

} // namespace sm
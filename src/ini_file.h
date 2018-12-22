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

#ifndef __INI_FILE_H__
#define __INI_FILE_H__

#include <stdio.h>

namespace sm {

	//! Interface class to read INI files
	class IniFileReaderInterface {
	public:
		virtual ~IniFileReaderInterface() = default;

		virtual void OnSection(const char* section_name) = 0;
		virtual void OnPair(const char* key, const char* value) = 0;
	};

	//! Class to handle INI files
	class IniFile {
	public:
		IniFile();
		~IniFile();

		bool OpenForWrite(const char* filename);
		bool OpenForRead(const char* filename);
		void Close();

		bool WriteSection(const char* section_name);
		bool WritePair(const char* key, const char* value);

		void Read(IniFileReaderInterface * reader);

	private:
		bool ReadLine(char text[256]);

		FILE * file_;
	};

} // namespace sm

#endif
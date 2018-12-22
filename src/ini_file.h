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
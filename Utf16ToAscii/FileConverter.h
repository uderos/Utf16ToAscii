#pragma once
class FileConverter
{
public:
	FileConverter();
	virtual ~FileConverter();

	void Convert(
		const std::string & fname_in,
		const std::string & fname_out);

private:

	unsigned int m_progress_percent;
	std::uintmax_t m_input_file_size;

	bool m_has_bom(const std::string & fname_in);

	void m_open_files(
		const std::string & fname_in, 
		const std::string & fname_out,
		std::wifstream & ifs, 
		std::ofstream & ofs);

	void m_convert(std::wifstream & ifs, std::ofstream & ofs);
	void m_read_file_size(const std::string & fname_in);
	void m_report_progress(const std::uintmax_t input_file_pos);
};


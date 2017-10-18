#include "stdafx.h"
#include "Config.h"
#include "FileConverter.h"

static const char PLACEHOLDER_CHAR{ '.' };

FileConverter::FileConverter() : 
	m_progress_percent(0),
	m_input_file_size(0)
{
}


FileConverter::~FileConverter()
{
}

void FileConverter::Convert(
	const std::string & fname_in, 
	const std::string & fname_out)
{
	std::wifstream ifs;
	std::ofstream ofs;

	m_open_files(fname_in, fname_out, ifs, ofs);

	m_convert(ifs, ofs);
}

bool FileConverter::m_has_bom(const std::string & fname_in)
{
	std::ifstream ifs(fname_in, std::ios::binary);
	uint8_t byte1, byte2;
	ifs >> byte1, byte2;

	const bool has_bom = (!ifs.eof()) && (byte1 == uint8_t(0xFF));

	return has_bom;
}

void FileConverter::m_open_files(
	const std::string & fname_in,
	const std::string & fname_out,
	std::wifstream & ifs,
	std::ofstream & ofs)
{

	if (!fs::exists(fname_in))
		throw std::runtime_error("Input file does not exists");

	m_read_file_size(fname_in);

	const bool has_bom = m_has_bom(fname_in);

	ifs.open(fname_in, std::ios::binary);
	if (!ifs)
		throw std::runtime_error("Unable to open input file");

	if (has_bom)
	{
		constexpr std::codecvt_mode mode =
			static_cast<std::codecvt_mode>(std::consume_header + std::little_endian);

		const std::locale my_locale(
			ifs.getloc(),
			new std::codecvt_utf16<wchar_t, 0x10ffff, mode>);

		ifs.imbue(my_locale);
	}
	else
	{
		constexpr std::codecvt_mode mode =
			static_cast<std::codecvt_mode>(std::little_endian);

		const std::locale my_locale(
			ifs.getloc(),
			new std::codecvt_utf16<wchar_t, 0x10ffff, mode>);

		ifs.imbue(my_locale);
	}

	ofs.open(fname_out);
	if (!ifs)
		throw std::runtime_error("Unable to open output file");
}

void FileConverter::m_read_file_size(const std::string & fname_in)
{
	constexpr auto NULL_SIZE = std::uintmax_t(-1);

	m_input_file_size = NULL_SIZE;

	try
	{
		m_input_file_size = fs::file_size(fname_in);
	}
	catch (...)
	{
	}

	if (m_input_file_size == std::uintmax_t(-1))
		throw std::runtime_error("Unable to read input file size");
}

void FileConverter::m_report_progress(
	const std::uintmax_t input_file_pos)
{
	const auto old_progress_percent = m_progress_percent;

	m_progress_percent = static_cast<unsigned>(100.0 *
		static_cast<double>(input_file_pos) /
		static_cast<double>(m_input_file_size));

	if (m_progress_percent != old_progress_percent)
		std::cout << "." << std::flush;
}


void FileConverter::m_convert(std::wifstream & ifs, std::ofstream & ofs)
{
	m_progress_percent = 0;

	const bool verbose_fiag = g_CONFIG.GetVerboseFlag();

	wchar_t wchar = 0;
	std::size_t ascii_counter = 0;
	std::size_t not_ascii_counter = 0;

	while (ifs.get(wchar))
	{
		const char charHi = char(wchar >> 8);
		const char charLow = char(wchar & 0x00FF);
		const bool is_ascii = (charHi == 0) && (charLow != 0);

		//if (verbose_fiag)
		//	std::cout
		//		<< " wchar=0x" << std::hex << int(wchar)
		//		<< " charHi=0x" << std::hex << int(charHi)
		//		<< " charLow=0x" << std::hex << int(charLow)
		//		<< std::dec
		//		<< std::endl;

		if (is_ascii)
		{
			++ascii_counter;
			ofs << charLow;
		}
		else
		{
			++not_ascii_counter;
			ofs << PLACEHOLDER_CHAR;
		}

		if (!ofs)
			throw std::runtime_error("Error writing output file");

		if (verbose_fiag)
			m_report_progress(ifs.tellg());
	}

	std::cout << std::endl;

	if (verbose_fiag)
		std::cout
			<< "nAscii=" << ascii_counter
			<< " nNotAscii=" << not_ascii_counter
			<< std::endl;
}



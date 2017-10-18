#include "stdafx.h"
#include "Config.h"


namespace po = boost::program_options;

Config * Config::m_instance_ptr = nullptr;

Config & Config::Instance()
{
	if (!m_instance_ptr)
	{
		m_instance_ptr = new Config();
	}

	return (*m_instance_ptr);
}

Config::Config() : m_verbose_flag(false)
{
}


void Config::ProcessCmdLine(const int argc, const char *argv[])
{
	// Declare the supported options.
	po::options_description desc("Utf16ToAscii - Allowed options");

	desc.add_options()
		("help,h", "Produce this help message")
		("input-file,i", po::value<std::string>()->required(), "Input file")
		("output-file,o", po::value<std::string>(), "Output file")
		("verbose,v", "Verbose logging on stdout")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return;
	}

	po::notify(vm);

	m_input_filename = vm["input-file"].as<std::string>();

	if (vm.count("output-file") > 0)
	{
		m_output_filename = vm["output-file"].as<std::string>();
	}
	else
	{
		m_output_filename = m_input_filename + std::string(".ascii.txt");
	}

	m_verbose_flag = (vm.count("verbose") > 0);


	if (m_verbose_flag)
		Dump();
}

void Config::Dump() const
{
	std::cout << "Configuration:"
		<< "\n\t input-file=" << m_input_filename
		<< "\n\t output-file=" << m_output_filename
		<< "\n\t verbose=" << m_verbose_flag
		<< std::endl;
}



const std::string & Config::GetInputFileName() const
{
	if (m_input_filename.empty())
		throw std::runtime_error("Input file name not defined");

	return m_input_filename;
}

const std::string & Config::GetOutputFileName() const
{
	if (m_input_filename.empty())
		throw std::runtime_error("Output file name not defined");

	return m_output_filename;
}

bool Config::GetVerboseFlag() const
{
	return m_verbose_flag;
}

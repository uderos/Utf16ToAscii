#pragma once

#define g_CONFIG (Config::Instance())
class Config
{
public:

	static Config & Instance();

	void ProcessCmdLine(const int argc, const char *argv[]);
	void Dump() const;

	const std::string & GetInputFileName() const;
	const std::string & GetOutputFileName() const;
	bool GetVerboseFlag() const;

private:

	Config();
	virtual ~Config() = default;

	static Config * m_instance_ptr;

	std::string m_input_filename;
	std::string m_output_filename;
	bool m_verbose_flag;

};


#include "udf.h"
#include "csv_utils.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace udf_vbm
{
	csv_file::csv_file(std::string filename)
	{
		file_ = new std::ifstream(filename);
	}

	csv_file::~csv_file()
	{
		file_->close();
	}

	std::vector<std::vector<real>> csv_file::read() const
	{
		std::vector<std::vector<real>> res;
		std::string line;
		std::vector<real> contents;
		while (std::getline(*file_, line))
		{
			if (line.empty())
				continue;
			auto cells = tokenize(line);
			for (auto& cell : cells)
			{
				auto num = std::stod(cell);
				contents.push_back(num);
			}
			res.push_back(contents);
			contents = {};
		}
		return res;
	}

	std::vector<std::string> csv_file::tokenize(const std::string input)
	{
		std::istringstream stream(input);
		std::string item;
		std::vector<std::string> res = {};
		while (std::getline(stream, item, ';'))
		{
			res.push_back(item);
		}
		return res;
	}
}

#ifndef csvutils
#define csvutils
#include <string>
#include <vector>
#include "udf.h"

namespace udf_vbm
{
	class csv_file
	{
	private:
		std::ifstream* file_;
	public:
		explicit csv_file(std::string filename);
		~csv_file();
		std::vector<std::vector<real>> read() const;
		static std::vector<std::string> tokenize(const std::string input);
	};
};
#endif

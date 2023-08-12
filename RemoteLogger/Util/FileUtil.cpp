#include "FileUtil.hpp"

#define VERBOSE 0

int readFileToQueue(const char (&fileName)[], cQueue &q)
{
	std::ifstream file{fileName};
	if (file.fail()) {
		#if VERBOSE
		std::cout << "file was not found.\nFILE: " << __FILE__ << "\nLINE: " << __LINE__;
		#endif 
		return -1;
	}
	std::string tmp;
	int count = 0;
	while (std::getline(file, tmp)) {
		q.enqueue(std::move(tmp));
		count++;
	}
	return count;
}
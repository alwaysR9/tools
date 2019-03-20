/*
 * read limited length line (with ifstream)
 * if the length of line(contain '\n') > max_len: return 1
 * if read eof, return -1
 * if ifstream encounted error, return -2,
 * else retuen 0, buff hold this line(contain '\0', do not contain '\n')
 * */

#include <fstream>
#include <iostream>
#include <string>

int get_limited_length_line(char* buff, const int max_len, std::ifstream & fin) {
	fin.getline(buff, max_len, '\n');
	if (fin.eof()) return -1;
	if (fin.bad()) return -2;
	if (fin.fail()) {
		while (1) {
			fin.clear(); // remove fail state of fin
			fin.getline(buff, max_len, '\n');
			if (fin.eof()) return -1;
			if (fin.bad()) return -2;
			if (!fin.fail()) break;
		}
		return 1;
	}
	return 0;
}

int main() {

	std::ifstream fin("xxx.txt");

	char buff[1024];
	while (1) {
		int ret = get_limited_length_line(buff, 4, fin);
		if (ret == -1) {
			std::cout << "read eof ." << std::endl;
			break;
		} else if (ret == -2) {
			std::cout << "stream error !!!" << std::endl;
			break;
		} else if (ret == 1) {
			std::cout << "this line is too long ..." << std::endl;
		} else {
			std::cout << "read a line:" << buff << std::endl;
		}
	}
	fin.close();

	return 0;
}

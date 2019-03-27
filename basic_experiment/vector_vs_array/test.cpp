#include <vector>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <iostream>
#include <sys/time.h>

// g++ -std=c++11 test.cpp -o x -O2
// ./x

/*********************/
/*       param       */
/*********************/
bool use_vector = true; // use vector or use array
int count = 3000000; // test count
const int ARRAY_NUM = 100; // the size of vector or the size of array


double time_diff(const timeval & b, const timeval & e) {
    return (e.tv_sec - b.tv_sec) + (e.tv_usec - b.tv_usec)*1.0 / 1000000.0;
}

void test_vector() {
	std::vector<int> s;
	for (int i = 0; i < ARRAY_NUM; ++ i) {
		s.emplace_back(i);;
	}
	if (count % (ARRAY_NUM / 2) == 0) {
		if (std::binary_search(s.begin(), s.end(), 10000, [](int a, int b){return a < b;}))
			std::cout << "Find" << std::endl;
		else
			std::cout << "Not Find" << std::endl;
	}
}

void test_array() {
	int* s = new int[ARRAY_NUM];
	for (int i = 0; i < ARRAY_NUM; ++ i)
		s[i] = i;

	if (count % (ARRAY_NUM / 2) == 0) {
		if (std::binary_search(s, s + ARRAY_NUM, 10000, [](int a, int b){return a < b;}))
			std::cout << "Find" << std::endl;
		else
			std::cout << "Not Find" << std::endl;
	}
	delete [] s;
}

int main() {

	timeval begin;
	timeval end;
	gettimeofday(&begin, NULL);

	while (count -- > 0) {
		if (use_vector) {
			test_vector();
		} 
		else {
			test_array();
		}
	}

	gettimeofday(&end, NULL);
	std::cout << time_diff(begin, end) << "s" << std::endl;

	return 0;
}

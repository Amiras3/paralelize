#include <iostream>
#include <vector>
#include <numeric>
#include <future>
#include <thread>
#include <functional>
#include <chrono>

using namespace std;

//paralelize accumulate for associative operations and with init equal to the neutral element
template<typename RandomIterator, typename T, typename BinaryOp>
auto accumulate_paralel(RandomIterator first, RandomIterator last, T init, BinaryOp op) -> T {

	auto dist = distance(first, last);
	auto numThreads = thread::hardware_concurrency() - 1;
	auto dim = (dist + numThreads - 1) / numThreads;
	
	vector<future<T>> result;
	result.reserve(numThreads);
	
	for(auto i = 0; i < numThreads; ++i) {
		auto start = dim * i;
		auto end = min(dim * (i + 1), dist);

		result.push_back(async(launch::async, [start, end, &first, init, &op]() {
			return accumulate(first + start, first + end, init, op);
		}));
	}

	for(auto& r : result) {
		auto t = r.get();
		init = op(init, t);
	}

	return init;
}

int main() {
	auto start = chrono::high_resolution_clock::now();
	vector<int> v(100'000'000, 1);
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << duration.count() << endl;
	
	start = chrono::high_resolution_clock::now();
	cout << accumulate_paralel(v.begin(), v.end(), 0, [](const auto& a, const auto& b) {
			return a + b;
		}) << endl;
	stop = chrono::high_resolution_clock::now();
	duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << duration.count() << endl;
	
	start = chrono::high_resolution_clock::now();
        cout << accumulate(v.begin(), v.end(), 0, [](const auto& a, const auto& b) {
                        return a + b;
                }) << endl;
        stop = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        cout << duration.count() << endl;

	return 0;
}			

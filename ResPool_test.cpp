#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "ResPool.hpp"


struct Conn
{
	Conn() {
		v = new int;
		*v = 100;
	}
	~Conn() {
		delete v;
	}
	int getv() {
		return *v;
	}
	int* v;
};

Conn* fun() {
	Conn* p = new Conn;
	return p;
}

int main()
{
	std::shared_ptr<ResPool<Conn>> pool = std::make_shared<ResPool<Conn>>(fun, 10);

	auto f = [&pool]()->void {
		for (int i = 0; i < 100; ++i) {
			std::unique_ptr<Conn> r = pool->Acquire();
			std::cout << r->getv() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			pool->Release(r);
			if (r) {
				std::cout << "error" << std::endl;
			}
		}
	};

	int tcnt = 3;
	std::vector<std::thread> v;
	for (int i = 0; i < tcnt; ++i) {
		v.emplace_back(std::thread(f));
	}

	for (auto&& t : v) {
		t.join();
	}

	return 0;
}
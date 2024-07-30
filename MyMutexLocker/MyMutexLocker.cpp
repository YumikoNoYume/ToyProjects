#include <mutex>
// for twsting
#include <iostream>
#include <thread>
#include <vector>


struct Locker {
	static void Lock(std::mutex& mutex) {
		mutex.lock();
	}
	static void Unlock(std::mutex& mutex) {
		mutex.unlock();
	}
};

template<typename type, typename mutex = std::mutex, typename locker = Locker>
class Locked {
public:

	class Guard {
	public:
		Guard() = default;
		Guard(Locked& locked) : locked_ref(locked) {
			locker::Lock(locked_ref.data_mutex);
		}

		template<typename... args>
		Guard(Locked& locked, args&&... arg) : locked_ref(locked) {
			locker::Lock(locked_ref.data_mutex, std::forward<args>(arg)...);
		}

		Guard(const Guard& other) = delete;
		Guard(Guard&& other) = delete;

		~Guard() {
			locker::Unlock(locked_ref.data_mutex);
		}
		type& operator * () {
			return locked_ref.data;
		}
		type* operator-> () {
			return &locked_ref.data;
		}
	private:
		Locked& locked_ref;
	};

	Locked() = default;

	Locked(type data) : data(data), data_mutex{} {}

	template<typename... args>
	Locked(args&&... param)
		: data{ std::forward<args>(param)... } , data_mutex{} {}

	Guard Lock() {
		return Guard(*this);
	} 

	template<typename... args>
	Guard Lock(args&&... arg) {
		return Guard(*this, std::forward<args>(arg)...);
	}

private:
	mutex data_mutex;
	type data;
};

// Testing 

Locked<std::vector<int>> data = {1, 2, 3, 4, 5};

void IncreaseVector(int begin, int end) {
	for (std::size_t index = begin; index <= end; index++) {
		auto guard = data.Lock();
		guard->push_back(index);
	}
}

int main() {
	std::thread thread1(IncreaseVector, 6, 10);
	std::thread thread2(IncreaseVector, 11, 15);
	thread1.join();
	thread2.join();
	auto guard = data.Lock();
	for (auto el : *guard) {
		std::cout << el << " ";
	}
	return 0;
}

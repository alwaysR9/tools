#ifndef __MESSAGE_QUEUE_HPP__
#define __MESSAGE_QUEUE_HPP__

#include <deque>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <assert.h>

template <typename T> 
class MessageDeque{
public:
	MessageDeque() : alive(1), high_water(0), cur_water(0), cur_wait_time(0)
	{
	}

	~MessageDeque()
	{
	}

	void resize(int queue_size)
	{
		q1.resize(queue_size);
	}

	void high_water_mark(unsigned int val)
	{
		assert(val > 100);
		high_water = val / 2;
		swap_water = high_water;
	}

	size_t size()
	{
		int num1 = 0, num2 = 0;
		{
			//std::unique_lock<std::mutex> lock(mu_producer);
			num1 = q1.size();
			//fprintf(stderr, "[queue] size1: %d\n", num1);
		}
		{
			//std::unique_lock<std::mutex> lock(mu_consumer);
			num2 = q2.size();
			//fprintf(stderr, "[queue] size2: %d\n", num2);
		}
		//fprintf(stderr, "[queue] size: %d %d\n", num1, num2);
		return num1 + num2;
	}

	void put_front(T *msg)
	{
		std::lock_guard<std::mutex> lock(mu_producer);
		
		if (alive) {
			q1.push_front(msg);
			if (q1.size() >= swap_water)
				cv_has_item.notify_all();
		}
	}

	void put(T *msg)
	{
		std::unique_lock<std::mutex> lock(mu_producer);
		while (alive && high_water && cur_water >= high_water)
			cv_has_room.wait(lock, [&]{return !alive || cur_water < high_water;});
		
		if (alive) { // cur_water < high_water || !high_water
			q1.push_back(msg);
			++ cur_water;
			if (q1.size() >= swap_water)
				cv_has_item.notify_all();
		} else { // !alive
			// do nothing
		}
	}

	T *get()
	{
		T *msg = NULL;

		try {
		std::lock_guard<std::mutex> lock2(mu_consumer);
		if (!q2.empty()) {
			msg = q2.front();
			q2.pop_front();
		} else {
			{
				// swap q1 and q2
				// q1 is empty after swap
				// q2 contain item after swap
				std::unique_lock<std::mutex> lock1(mu_producer);
				if (q1.size() >= swap_water) {
					q1.swap(q2);
					cur_water = 0;
					cv_has_room.notify_all();
				} else {
					while (q2.size() == 0) {
						while(true) {
							bool res = cv_has_item.wait_for(lock1, std::chrono::milliseconds(10),
															[&]{ return !alive || q1.size() >= swap_water; });
							if (!res) { // wait timeout
								cur_wait_time += 10;
								if (cur_wait_time >= MAX_WAIT_TIME) {
									cur_wait_time = 0;
									break;
								}
							} else {
								if (!alive)
									return NULL;
								else { // q1.size() >= swap_water
									break;
								}
							}
						}
						q1.swap(q2);
						cur_water = 0;
						cv_has_room.notify_all();
					}
				}
			}
			msg = q2.front();
			q2.pop_front();
		}
		} catch (std::exception & e) {
			fprintf(stderr, "[message_deque] exp: %s\n", e.what());
		}

		return msg;
	}
	void flush()
	{
		alive = 0;
		cv_has_room.notify_all();
		cv_has_item.notify_all();
	}
private:
	std::deque<T *> q1; // for producer
	std::deque<T *> q2; // for consumer
	unsigned int high_water;
	unsigned int swap_water;
	unsigned int cur_water; // limit put(), not limit put_front()
	int alive;
	int cur_wait_time;
	std::mutex mu_producer;
	std::mutex mu_consumer;
	std::condition_variable cv_has_item;
	std::condition_variable cv_has_room;

	const static int MAX_WAIT_TIME = 1000;
};
#endif //__MESSAGE_QUEUE_HPP__

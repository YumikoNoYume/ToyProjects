#include <iostream>

class IntVector
{
public:
	class Iterator
	{
	public:
		Iterator() : data(nullptr) {};
		explicit Iterator(int* data) : data(data) {};

		int& operator * ()
		{
			return *data;
		}
		void operator ++ ()
		{
			data += 1;
		}
		bool operator == (const Iterator& other) const
		{
			return data == other.data;
		}
		bool operator != (const Iterator& other) const
		{
			return !(*this == other);
		}
	private:
		int* data;
	};

	IntVector() : capacity(0), size(0), data(nullptr) {}
	~IntVector()
	{
		delete[] data;
	}
	IntVector(const IntVector& other) : size(other.size), capacity(other.capacity)
	{
		data = new int[capacity];
		if (size != 0)
		{
			for (std::size_t index = 0; index < size; index++)
			{
				data[index] = other.data[index];
			}
		}
	}
	IntVector(IntVector&& other) noexcept
	{
		size = other.size; other.size = 0;
		capacity = other.capacity; other.capacity = 0;
		data = other.data; other.data = nullptr;
	}

	IntVector& operator = (const IntVector& other)
	{
		IntVector new_vector{ other };
		Swap(*this, new_vector);
		return *this;
	}
	IntVector& operator = (IntVector&& other) noexcept
	{
		if (this != &other)
		{
			delete[] data;
			data = other.data; other.data = nullptr;
			capacity = other.capacity; other.capacity = 0;
			size = other.size; other.size = 0;
		}
		return *this;
	}
	int& operator [] (std::size_t index)
	{
		return data[index];
	}
	const int& operator [] (std::size_t index) const
	{
		return data[index];
	}

	void PushBack(int val)
	{
		if (size >= capacity)
		{
			if (capacity == 0) { Reserve(1); }
			else { Reserve(capacity * 2); }
		}
		data[size] = val;
		size++;
	}
	void Reserve(std::size_t newCapacity)
	{
		int* new_data = new int[newCapacity];
		capacity = newCapacity;
		Copy(new_data, data);
		delete[] data;
		data = new_data;
	}
	std::size_t GetSize() const
	{
		return size;
	}
	std::size_t GetCapacity() const
	{
		return capacity;
	}

	Iterator begin()
	{
		return Iterator(data);
	}
	Iterator end()
	{
		return Iterator(data + size);
	}

private:
	static void Swap(IntVector& vector1, IntVector& vector2)
	{
		std::swap(vector1.capacity, vector2.capacity);
		std::swap(vector1.size, vector2.size);
		std::swap(vector1.data, vector2.data);
	}
	void Copy(int* data1, int* data2)
	{
		for (std::size_t index = 0; index < size; index++)
		{
			data1[index] = data2[index];
		}
	}
	int* data;
	std::size_t size;
	std::size_t capacity;
};

int main()
{
	IntVector vector;
	vector.PushBack(12);
	vector.PushBack(15);
	vector.PushBack(10);
	vector.PushBack(4);
	std::size_t vector_size = vector.GetSize();
	std::size_t vector_capacity = vector.GetCapacity();
	std::cout << vector_size << " " << vector_capacity << " ";
	for (IntVector::Iterator i = vector.begin(); i != vector.end(); ++i)
	{
		*i += 1;
		std::cout << *i << " ";
	}
	return 0;
}
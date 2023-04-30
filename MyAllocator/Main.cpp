#include <iostream>
#include <numeric>
#include <cassert>

using byte = char;

class Bitset
{
public:
	explicit Bitset(std::size_t bits) : number_of_bits(bits)
	{
		std::size_t size = 0;
		unsigned int modulo = number_of_bits % 8;
		if (modulo == 0) { size = number_of_bits / 8; }
		else { size = number_of_bits / 8 + 1; }
		bytes = new byte[size];
		for (std::size_t index = 0; index < size; index++)
		{
			bytes[index] = 0b11111111;
		}
	}
	~Bitset()
	{
		delete[] bytes;
	}
	bool Get(std::size_t bit) const
	{
		std::size_t byte_index = bit / 8;
		unsigned int bit_index = bit % 8;
		byte temp = bytes[byte_index];
		temp |= (1 << bit_index);
		return bytes[byte_index] == temp;
	}
	void Set(std::size_t bit, bool setting)
	{
		std::size_t byte_index = bit / 8;
		unsigned int bit_index = bit % 8;
		if (setting) { bytes[byte_index] |= (1 << bit_index); } 
		else { bytes[byte_index] &= ~(1 << bit_index); }
	}
private:
	std::size_t number_of_bits;
	byte* bytes;
};

class Allocator {
public:
	Allocator(std::size_t max) : max_memory(max), is_free(max)
	{
		memory = new byte[max];
		SetFreeOrNot(true, max_memory - 1, 0);
	}
	~Allocator()
	{
		delete[] memory;
	}

	void* Malloc(std::size_t size_of_type)
	{
		if (sizeof(AllocatorHeader<uint8_t>) + size_of_type < std::numeric_limits<uint8_t>::max())
		{
			AllocatorHeader<uint8_t>* header;
			std::size_t memory_pointer = FindUnusedMemory(size_of_type, sizeof(*header));
			if (memory_pointer == max_memory)
			{
				return nullptr;
			}
			header = reinterpret_cast<AllocatorHeader<uint8_t>*>(&memory[memory_pointer]);
			header->fragment_size = size_of_type + sizeof(*header);
			header->fragment_pointer = memory_pointer;
			header->header_size = '0';
			SetFreeOrNot(false, size_of_type + sizeof(*header), memory_pointer);
			return &memory[memory_pointer + sizeof(*header)];
		}
		if (sizeof(AllocatorHeader<uint16_t>) + size_of_type < std::numeric_limits<uint16_t>::max())
		{
			AllocatorHeader<uint16_t>* header;
			std::size_t memory_pointer = FindUnusedMemory(size_of_type, sizeof(*header));
			if (memory_pointer == max_memory)
			{
				return nullptr;
			}
			header = reinterpret_cast<AllocatorHeader<uint16_t>*>(&memory[memory_pointer]);
			header->fragment_size = size_of_type + sizeof(*header);
			header->fragment_pointer = memory_pointer;
			header->header_size = '1';
			SetFreeOrNot(false, size_of_type + sizeof(*header), memory_pointer);
			return &memory[memory_pointer + sizeof(*header)];
		}
		if (sizeof(AllocatorHeader<uint32_t>) + size_of_type < std::numeric_limits<uint32_t>::max())
		{
			AllocatorHeader<uint32_t>* header;
			std::size_t memory_pointer = FindUnusedMemory(size_of_type, sizeof(*header));
			if (memory_pointer == max_memory)
			{
				return nullptr;
			}
			header = reinterpret_cast<AllocatorHeader<uint32_t>*>(&memory[memory_pointer]);
			header->fragment_size = size_of_type + sizeof(*header);
			header->fragment_pointer = memory_pointer;
			header->header_size = '2';
			SetFreeOrNot(false, size_of_type + sizeof(*header), memory_pointer);
			return &memory[memory_pointer + sizeof(*header)];
		}
		if (sizeof(AllocatorHeader<uint64_t>) + size_of_type < std::numeric_limits<uint64_t>::max())
		{
			AllocatorHeader<uint64_t>* header;
			std::size_t memory_pointer = FindUnusedMemory(size_of_type, sizeof(*header));
			if (memory_pointer == max_memory)
			{
				return nullptr;
			}
			header = reinterpret_cast<AllocatorHeader<uint64_t>*>(&memory[memory_pointer]);
			header->fragment_size = size_of_type + sizeof(*header);
			header->fragment_pointer = memory_pointer;
			header->header_size = '3';
			SetFreeOrNot(false, size_of_type + sizeof(*header), memory_pointer);
			return &memory[memory_pointer + sizeof(*header)];
		}
		assert(false);
		return nullptr;
	}
	void Free(void* mem)
	{
		byte* fragment = reinterpret_cast<byte*>(mem);
		if (*(fragment - 1) == '0')
		{
			AllocatorHeader<uint8_t>* header = reinterpret_cast<AllocatorHeader<uint8_t>*>(fragment - sizeof(AllocatorHeader<uint8_t>));
			SetFreeOrNot(true, header->fragment_size, header->fragment_pointer);
		}
		if (*(fragment - 1) == '1')
		{
			AllocatorHeader<uint16_t>* header = reinterpret_cast<AllocatorHeader<uint16_t>*>(fragment - sizeof(AllocatorHeader<uint16_t>));
			SetFreeOrNot(true, header->fragment_size, header->fragment_pointer);
		}
		if (*(fragment - 1) == '2')
		{
			AllocatorHeader<uint32_t>* header = reinterpret_cast<AllocatorHeader<uint32_t>*>(fragment - sizeof(AllocatorHeader<uint32_t>));
			SetFreeOrNot(true, header->fragment_size, header->fragment_pointer);
		}
		if (*(fragment - 1) == '3')
		{
			AllocatorHeader<uint64_t>* header = reinterpret_cast<AllocatorHeader<uint64_t>*>(fragment - sizeof(AllocatorHeader<uint64_t>));
			SetFreeOrNot(true, header->fragment_size, header->fragment_pointer);
		}
	}
private:
	std::size_t FindUnusedMemory(std::size_t size_of_type, std::size_t size_of_header) const
	{
		for (std::size_t pointer = 0; pointer < max_memory; pointer++)
		{
			if (is_free.Get(pointer))
			{
				std::size_t fragment_size = 0;
				while (is_free.Get(pointer + fragment_size))
				{
					fragment_size++;
					if (fragment_size >= size_of_type + size_of_header) { return pointer; }
				}
				pointer += fragment_size;
			}
		}
		return max_memory;
	}

	void SetFreeOrNot(bool setting, std::size_t fragment_size, std::size_t pointer)
	{
		for (std::size_t i = 0; i < fragment_size; i++)
		{
			is_free.Set(pointer + i, setting);
		}
	}

	template <typename T>
	struct AllocatorHeader
	{
		std::size_t fragment_pointer;
		T fragment_size;
		unsigned char header_size;
	};
	std::size_t max_memory;
	Bitset is_free;
	byte* memory;
};

int main()
{
	Allocator allocator(100);
	int* mem_fragment1 = reinterpret_cast<int*>(allocator.Malloc(sizeof(int)));
	int* mem_fragment2 = reinterpret_cast<int*>(allocator.Malloc(sizeof(int)));
	allocator.Free(mem_fragment1);
	double* mem_fragment4 = reinterpret_cast<double*>(allocator.Malloc(sizeof(double)));
	return 0;
}

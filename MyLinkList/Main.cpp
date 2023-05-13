#include <iostream>

class List
{
public:
	struct Entry
	{
		int data;
		Entry* previous;
		Entry* next;
	};
	class Iterator
	{
	public:
		Iterator() : pointer(nullptr) {}
		explicit Iterator(Entry* pointer) : pointer(pointer) {}

		int& operator * () const
		{
			return pointer->data;
		}
		void operator ++ ()
		{
			pointer = pointer->next;
		}
		void operator -- ()
		{
			pointer = pointer->previous;
		}
		bool operator == (const Iterator& other) const
		{
			return pointer == other.pointer;
		}
		bool operator != (const Iterator& other) const
		{
			return !(*this == other);
		}
		Entry* GetPointer()
		{
			return pointer;
		}
	private:
		Entry* pointer;
	};
	class RIterator : public Iterator
	{
	public:
		RIterator() : Iterator() {}
		explicit RIterator(Entry* pointer) : Iterator(pointer) {}
		void operator ++ ()
		{
			Iterator::operator--();
		}
		void operator -- ()
		{
			Iterator::operator++();
		}
	};
	class CIterator
	{
	public:
		CIterator() : pointer(nullptr) {}
		explicit CIterator(const Entry* pointer) : pointer(pointer) {}

		const int& operator * () const
		{
			return pointer->data;
		}
		void operator ++ ()
		{
			pointer = pointer->next;
		}
		void operator -- ()
		{
			pointer = pointer->previous;
		}
		bool operator == (const CIterator& other) const
		{
			return pointer == other.pointer;
		}
		bool operator != (const CIterator& other) const
		{
			return !(*this == other);
		}
		const Entry* GetPointer()
		{
			return pointer;
		}
	private:
		const Entry* pointer;
	};
	class CRIterator : public CIterator
	{
	public:
		CRIterator() : CIterator() {}
		explicit CRIterator(const Entry* pointer) : CIterator(pointer) {}
		void operator ++ ()
		{
			CIterator::operator--();
		}
		void operator -- ()
		{
			CIterator::operator++();
		}
	};

	List() : first(nullptr), last(nullptr), list_size(0) {}
	List(const List& other) : first(nullptr), last(nullptr), list_size(0)
	{
		for (auto i = other.Begin(); i != other.End(); ++i)
		{
			PushBack(*i);
		}
	}
	List(List&& other) noexcept : List()
	{
		Swap(*this, other);

	}
	List(std::initializer_list<int> values) : List()
	{
		for (auto i = values.begin(); i != values.end(); ++i)
		{
			PushBack(*i);
		}
	}
	~List()
	{
		Entry* current = first;
		while (current)
		{
			Entry* temp = current;
			current = current->next;
			delete temp;
		}
	}

	List& operator = (const List& other)
	{
		List new_list{ other };
		Swap(*this, new_list);
		return *this;
	}
	List& operator = (List&& other) noexcept
	{
		Swap(*this, other);
		return *this;
	}

	void PushFront(int new_data)
	{
		Entry* entry = new Entry;
		entry->data = new_data;
		entry->previous = nullptr;
		if (list_size == 0)
		{
			entry->next = nullptr;
			last = entry;
			first = entry;
			list_size++;
			return;
		}
		first->previous = entry;
		entry->next = first;
		first = entry;
		list_size++;
	}
	void PopFront()
	{
		Pop(Begin());
	}
	void PushBack(int new_data)
	{
		Entry* entry = new Entry;
		entry->data = new_data;
		entry->next = nullptr;
		if (list_size == 0)
		{
			entry->previous = nullptr;
			first = entry;
			last = entry;
			list_size++;
			return;
		}
		last->next = entry;
		entry->previous = last;
		last = entry;
		list_size++;
	}
	void PopBack()
	{
		Pop(RBegin());
	}
	void Pop(Iterator iterator)
	{
		Entry* next = iterator.GetPointer()->next;
		Entry* previous = iterator.GetPointer()->previous;
		delete iterator.GetPointer();
		list_size--;
		if (iterator.GetPointer() == first)
		{
			first = next;
			first->previous = nullptr;
		}
		else if (iterator.GetPointer() == last)
		{
			last = previous;
			last->next = nullptr;
		}
		else
		{
			previous->next = next;
			next->previous = previous;
		}
	}

	CIterator Begin() const
	{
		return CIterator(first);
	}
	CIterator End() const
	{
		return CIterator(last->next);
	}
	CRIterator RBegin() const
	{
		return CRIterator(last);
	}
	CRIterator REnd() const
	{
		return CRIterator(first->previous);
	}
	Iterator Begin()
	{
		return Iterator(first);
	}
	Iterator End()
	{
		return Iterator(last->next);
	}
	RIterator RBegin()
	{
		return RIterator(last);
	}
	RIterator REnd()
	{
		return RIterator(first->previous);
	}

	std::size_t GetSize() const
	{
		return list_size;
	}
	int GetFirst() const
	{
		return first->data;
	}
	int GetLast() const
	{
		return last->data;
	}
	static void Swap(List& list1, List& list2)
	{
		std::swap(list1.list_size, list2.list_size);
		std::swap(list1.first, list2.first);
		std::swap(list1.last, list2.last);
	}

	void Reverse()
	{
		first = last;
		Entry* current = first;
		while (current)
		{
			Entry* temp = current->next;
			current->next = current->previous;
			current->previous = temp;
			last = current;
			current = current->next;
		}
	}
private:
	Entry* first;
	Entry* last;
	std::size_t list_size;
};

int main()
{
	List test_list0, test_list2, test_list4;
	std::cout << "Iterator, PushFront and PushBAck functions test (test_list0): ";
	test_list0.PushFront(12);
	test_list0.PushFront(11);
	test_list0.PushBack(13);
	test_list0.PushBack(14);
	test_list0.PushFront(10);
	test_list0.PushBack(15);
	for (auto i = test_list0.Begin(); i != test_list0.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nCopy constructor and copy = operator test:\ntest_list1: ";
	List test_list1{ test_list0 };
	test_list2 = test_list0;
	for (auto i = test_list1.Begin(); i != test_list1.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\ntest_list2: ";
	for (auto i = test_list2.Begin(); i != test_list2.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nMove constructor and move = operator test:\ntest_list3 (from test_list0): ";
	List test_list3{ (List&&)test_list0 };
	test_list4 = (List&&)test_list1;
	for (auto i = test_list3.Begin(); i != test_list3.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\ntest_list4 (from test_list1): ";
	for (auto i = test_list4.Begin(); i != test_list4.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nReversed iterator, Pop, PopFront and PopBack functions test (test_list2): ";
	test_list2.PopBack();
	test_list2.PopFront();
	auto i = test_list2.RBegin();
	for (std::size_t pos = 0; pos < 3; ++pos)
	{
		++i;
	}
	test_list2.Pop(i);
	for (auto i = test_list2.RBegin(); i != test_list2.REnd(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nGetFirst anf GetLast functions test (test_list2):\nfirst element devided by 3: " << test_list2.GetFirst() / 3;
	std::cout << "\nlast element + 1: " << test_list2.GetLast() + 1;
	std::cout << "\nInitializer list constructor test (test_list5): ";
	List test_list5 = { 1, 2, 3, 4, 5, 6, 7, 8 };
	for (auto i = test_list5.Begin(); i != test_list5.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nGet size function test (test_list5): " << test_list5.GetSize();
	std::cout << "\nReverse function and Reverse iterator test (test_list5):\nReverse function: ";
	test_list5.Reverse();
	for (auto i = test_list5.Begin(); i != test_list5.End(); ++i)
	{
		std::cout << *i << " ";
	}
	std::cout << "\nReverse iterator: ";
	for (auto i = test_list5.RBegin(); i != test_list5.REnd(); ++i)
	{
		std::cout << *i << " ";
	}
	return 0;
}

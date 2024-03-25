#include <iostream>
#include <type_traits>


template <typename type>
class SharedPtr {
public:
	SharedPtr() = default;

	SharedPtr(type* d) : data(d) {
		refs = new std::size_t{ 1 };
	}

	template <typename type2,
		typename = typename std::enable_if_t<std::is_convertible_v<type2*, type*>>>
	SharedPtr(const SharedPtr<type2>& ptr) {
		data = ptr.Get();
		refs = ptr.GetRefsCount();
		(*refs)++;
	}

	SharedPtr(const SharedPtr& ptr) {
		data = ptr.Get();
		refs = ptr.GetRefsCount();
		(*refs)++;
	}

	template <typename type2,
		typename = typename std::enable_if_t<std::is_convertible_v<type2*, type*>>>
	SharedPtr(SharedPtr<type2>&& ptr) noexcept {
		Swap(*this, ptr);
	}

	SharedPtr(SharedPtr&& ptr) noexcept {
		Swap(*this, ptr);
	}

	~SharedPtr() {
		(*refs)--;
		if (*refs == 0) {
			delete data;
			delete refs;
		}
	}

	template <typename type2,
		typename = typename std::enable_if_t<std::is_convertible_v<type2*, type*>>>
	SharedPtr<type>& operator = (const SharedPtr<type2>& ptr) {
		SharedPtr newSharedPtr{ ptr };
		Swap(*this, newSharedPtr);
		return *this;
	}

	template <typename type2,
		typename = typename std::enable_if_t<std::is_convertible_v<type2*, type*>>>
	SharedPtr<type>& operator = (SharedPtr<type2>&& ptr) noexcept {
		Swap(*this, ptr);
		return *this;
	}

	SharedPtr<type>& operator = (const SharedPtr& ptr) {
		SharedPtr newSharedPtr{ ptr };
		Swap(*this, newSharedPtr);
		return *this;
	}

	SharedPtr<type>& operator = (SharedPtr&& ptr) noexcept {
		Swap(*this, ptr);
		return *this;
	}

	type* Get() const {
		return data;
	}
	std::size_t* GetRefsCount() const {
		return refs;
	}
private:
	template<typename type1, typename type2>
	static void Swap(SharedPtr<type1>& ptr1, SharedPtr<type2>& ptr2) {
		std::swap(ptr1.data, ptr2.data);
		std::swap(ptr1.refs, ptr2.refs);
	}
	type* data = nullptr;
	std::size_t* refs = nullptr;
};

class Data {
public:
	Data() : data(88) {}
	~Data() {}
	int GetData() {
		return data;
	}
private:
	int data;
};

struct Data2 : Data {};

int main() {
	SharedPtr<Data> d1{ new Data };
	SharedPtr<Data2> d2{ new Data2 };
	d1 = d2;
	return 0;
}
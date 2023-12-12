#include <iostream>
#include <exception>
#include <algorithm> 

template <class type> 
class TString;

template <class type>
typename TString<type>::RandomAccessIterator operator+ (long long int num, const typename TString<type>::RandomAccessIterator& rai);

template <class type>
class TString {
public:
	class RandomAccessIterator : public std::iterator<std::random_access_iterator_tag, type> {
		using Super = std::iterator<std::random_access_iterator_tag, type>;
	public:
		RandomAccessIterator() : data(nullptr) {}
		explicit RandomAccessIterator(type* pointer, std::size_t index) : data(pointer) {}

		RandomAccessIterator& operator++ () { 
			data += 1; 
			return *this; 
		}
		RandomAccessIterator& operator-- () { 
			data -= 1; 
			return *this;
		}
		RandomAccessIterator operator++ (int) {
			auto old = *this;
			data += 1;
			return old;
		}
		RandomAccessIterator operator-- (int) { 
			auto old = *this; 
			data -= 1; 
			return old; 
		}
		Super::difference_type operator- (const RandomAccessIterator& other) const {
			return data - other.data;
		}
		Super::reference operator* () const {
			return *data;
		}
		bool operator== (const RandomAccessIterator& other) const {
			return data == other.data;
		}
		bool operator!= (const RandomAccessIterator& other) const {
			return !(*this == other);
		}
		bool operator< (const RandomAccessIterator& other) const {
			return other - *this > 0;
		}
		bool operator> (const RandomAccessIterator& other) const {
			return *this - other > 0;
		}
		bool operator<= (const RandomAccessIterator& other) const {
			return !(*this > other);
		}
		bool operator>= (const RandomAccessIterator& other) const {
			return !(*this < other);
		}
		RandomAccessIterator operator+ (long long int num) const {
			RandomAccessIterator i{ *this };
			i += num;
			return i;
		}
		RandomAccessIterator operator- (long long int num) const {
			RandomAccessIterator i{ *this };
			i -= num;
			return i;
		}
		RandomAccessIterator& operator+= (long long int num) {
			data += num;
			return *this;
		}
		RandomAccessIterator& operator-= (long long int num) {
			data -= num;
			return *this;
		}
		Super::reference operator[] (std::size_t index) {
			return *(*this + index);
		}
	private:
		type* data;
	};
	class OutOfRange : public std::exception {
	public:
		OutOfRange() {}
		const char* what() const override {
			return "Out of range";
		}
	};
	TString() : smallString(), isSmallString(true) {
		smallString.data[0] = '\0';
		smallString.size = 1;
	}
	TString(const type* stringToPut) : TString() {
		Append(stringToPut);
	}
	TString(const type charToPut) : TString() {
		PushBack(charToPut);
	}
	TString(const TString& other) : TString() {
		Append(other);
	}
	TString(TString&& other) noexcept : TString() {
		Swap(*this, other);
	}
	~TString() { 
		Clear();
	}

	TString& operator = (const TString& other) {
		TString newString{ other };
		Swap(*this, newString);
		return *this;
	}
	TString& operator = (TString&& other) noexcept {
		Swap(*this, other);
		return *this;
	}

	void Reserve(std::size_t newCapacity) { 
		if ((isSmallString && newCapacity < smallStringCapacity) || (!isSmallString && newCapacity < largeString.size)) { 
			return; 
		}
		if (isSmallString) {
			TransformSmallToLarge(newCapacity);
		}
		else {
			type* newData = new type[newCapacity];
			largeString.capacity = newCapacity;
			CopyData(newData, largeString.data, largeString.size);
			delete[] largeString.data;
			largeString.data = newData;
		}
	}
	void Append(const TString& stringToPut) {
		Append(stringToPut.GetCString());
	}
	void Append(const type* stringToPut) {
		std::size_t stringToPutSize = std::char_traits<type>::length(stringToPut);
		if (isSmallString && (stringToPutSize + smallString.size) < smallStringCapacity) {
			std::size_t size = smallString.size;
			Push(stringToPut, stringToPutSize, smallString.data, size);
			smallString.size = size;
			return;
		}
		if (isSmallString) {
			TransformSmallToLarge(smallStringCapacity + stringToPutSize + 1);
		}
		else if ((stringToPutSize + largeString.size) > largeString.capacity) {
			Reserve(largeString.capacity + stringToPutSize + 1);
		}
		Push(stringToPut, stringToPutSize, largeString.data, largeString.size);
	}
	void PopBack() {
		if (!isSmallString) {
			largeString.size--;
			largeString.data[largeString.size - 1] = '\0'; 
			if (largeString.size <= smallStringCapacity) {
				TransformLargeToSmall();
			}
			return;
		}
		if (smallString.size == 1) {
			throw OutOfRange();
		}
		smallString.size--;
		smallString.data[smallString.size - 1] = '\0'; 	
	}
	void PushBack(const type charToPut) {
		type stringToPut[2];
		stringToPut[0] = charToPut;
		stringToPut[1] = '\0';
		Append(stringToPut);
	}
	static void Swap(TString& string1, TString& string2) noexcept
	{
		if (string2.isSmallString) {
			std::swap(string1.smallString.size, string2.smallString.size);
			std::swap(string1.smallString.data, string2.smallString.data);
		}
		else {
			std::swap(string1.largeString.size, string2.largeString.size);
			std::swap(string1.largeString.capacity, string2.largeString.capacity);
			std::swap(string1.largeString.data, string2.largeString.data);
		}
		std::swap(string1.isSmallString, string2.isSmallString);
	}
	void Clear() {
		if (!isSmallString) {
			delete[] largeString.data;
			isSmallString = true;
		}
		smallString.size = 1;
		smallString.data[0] = '\0';
	}

	const type* GetCString() const {
		if (isSmallString) {
			return smallString.data;
		}
		return largeString.data;
	}
	std::size_t GetSize() const noexcept {
		if (isSmallString) {
			return smallString.size - 1;
		}
		return largeString.size - 1;
	}
	std::size_t GetCapacity() const noexcept {
		if (isSmallString) {
			return smallStringCapacity;
		}
		return largeString.capacity;
	}

	RandomAccessIterator begin() {
		if (isSmallString) {
			return RandomAccessIterator(smallString.data, 0);
		}
		return RandomAccessIterator(largeString.data, 0);
	}
	RandomAccessIterator end() { 
		if (isSmallString) {
			return RandomAccessIterator(smallString.data + smallString.size - 1, smallString.size - 1);
		}
		return RandomAccessIterator(largeString.data + largeString.size - 1, largeString.size - 1);
	}
private:
	// Methods	
	static void CopyData(type* string1, const type* string2, std::size_t string2Size) {
		for (std::size_t index = 0; index < string2Size; index++)
		{
			string1[index] = string2[index];
		}
	}
	void TransformSmallToLarge(std::size_t newCapacity) {
		isSmallString = false;
		if (smallString.size > 1) {
			type* tempData = new type[newCapacity];
			CopyData(tempData, smallString.data, smallString.size);
			largeString.size = smallString.size;
			largeString.capacity = newCapacity;			
			largeString.data = tempData;
			return;
		}
		largeString.size = smallString.size;
		largeString.capacity = newCapacity;
		largeString.data = new type[largeString.capacity];
		largeString.data[0] = '\0';
	}
	void TransformLargeToSmall() {
		isSmallString = true;
		type* data = largeString.data;
		char size = largeString.size;
		CopyData(smallString.data, data, size);
		smallString.size = size;
		delete[] data;
	}
	static void Push(const type* stringToPut, std::size_t stringToPutSize, type* mainString, std::size_t& stringToPutInSize) {
		stringToPutInSize--;
		for (std::size_t index1 = stringToPutInSize, index2 = 0; index2 < stringToPutSize; index1++, index2++) {
			mainString[index1] = stringToPut[index2];
			stringToPutInSize++;
		}
		mainString[stringToPutInSize] = '\0';
		stringToPutInSize++;
	}

	// Structs, classes and static data
	struct LargeString {
		type* data = nullptr;
		std::size_t size = 0;
		std::size_t capacity = 0;
	};
	static const std::size_t smallStringCapacity = (sizeof(LargeString) / sizeof(type)) - 1;
	struct SmallString {
		char size = 0;
		type data[smallStringCapacity];
	};

	// Data
	bool isSmallString;
	union {
		LargeString largeString;
		SmallString smallString;
	};
};

template <class type>
typename TString<type>::RandomAccessIterator operator+ (long long int num, const typename TString<type>::RandomAccessIterator& rai) {
	return rai + num;
}

using String = TString<char>;
using WString = TString<wchar_t>;

int main() {
	try {
		String ssSample{ "I am Yumiko" }; // Constructor1 check, Append(const char*) check, Push() check;
		String lsSample{ "I am Yumiko and my dream is to be a programmer!!" }; // TransformSmallToLarge() check;
		String charSample{ '!' }; // Constructor2 check;
		ssSample.Append(charSample); // Append(const String&) check
		ssSample.PushBack(' '); // PushBack() check;
		lsSample.PopBack(); //PopBack() check;
		String testString1, testString2, testString3, testString4; // Default constructor check;
		String ssCopy{ ssSample }; // Copy constructor check;
		String lsCopy{ lsSample };
		testString1 = ssCopy; // operator= (const String& other) check, Swap() check;
		testString2 = lsCopy;
		testString3 = (String&&)testString1; // operator= (String && other) check;
		testString4 = (String&&)testString2;
		String ssMove{ testString3 }; // Move constructor check;
		String lsMove{ testString4 };
		// Empty object constructors check;
		String empty1{};
		String empty2{ empty1 };
		String empty3{ (String&&)empty2 };
		// Reserve() check;
		ssCopy.Reserve(20); 
		ssCopy.Reserve(40);
		empty1.Reserve(40);
		lsCopy.Reserve(100); // CopyData() check;
		String testString5{ "aaaaabbbbbcccccdddddeeee" };
		testString5.PopBack();
		testString5.PopBack(); // TransformLargeToSmall() check;
		ssCopy.Clear(); // Clear() check;
		lsCopy.Clear();
		//empty3.PopBack();
		//empty3.PopBack(); // Exception check;
		std::wcout << "String: " << lsSample.GetCString()
			<< "\nString size: " << lsSample.GetSize()
			<< "\nString capacity: " << lsSample.GetCapacity()
			<< "\nString: " << ssSample.GetCString()
			<< "\nString size: " << ssSample.GetSize()
			<< "\nString capacity: " << ssSample.GetCapacity()
			<< "\nString: " << charSample.GetCString()
			<< "\nString size: " << charSample.GetSize()
			<< "\nString capacity: " << charSample.GetCapacity()
			<< "\nString: " << empty1.GetCString()
			<< "\nString size: " << empty1.GetSize()
			<< "\nString capacity: " << empty1.GetCapacity() << "\n";
		std::sort(testString4.begin(), testString4.end()); // RAI check;
		std::wcout << testString4.GetCString();
	}
	catch (String::OutOfRange e) {
		std::wcout << "Exception caught: " << e.what() << '\n';
	}
	try {
		WString wssSample{ L"I am Yumiko" }; // Constructor1 check, Append(const char*) check, Push() check;
		WString wlsSample{ L"I am Yumiko and my dream is to be a programmer!!" }; // TransformSmallToLarge() check;
		WString wcharSample{ L'!' }; // Constructor2 check;
		wssSample.Append(wcharSample); // Append(const String&) check
		wssSample.PushBack(L' '); // PushBack() check;
		wlsSample.PopBack(); //PopBack() check;
		WString wtestString1, wtestString2, wtestString3, wtestString4; // Default constructor check;
		WString wssCopy{ wssSample }; // Copy constructor check;
		WString wlsCopy{ wlsSample };
		wtestString1 = wssCopy; // operator= (const String& other) check, Swap() check;
		wtestString2 = wlsCopy;
		wtestString3 = (WString&&)wtestString1; // operator= (String && other) check;
		wtestString4 = (WString&&)wtestString2;
		WString wssMove{ wtestString3 }; // Move constructor check;
		WString wlsMove{ wtestString4 };
		// Empty object constructors check;
		WString wempty1{};
		WString wempty2{ wempty1 };
		WString wempty3{ (WString&&)wempty2 };
		// Reserve() check;
		wssCopy.Reserve(20);
		wssCopy.Reserve(40);
		wempty1.Reserve(40);
		wlsCopy.Reserve(100); // CopyData() check;
		WString wtestString5{ L"aaaaabbbbbcccccdddddeeee" };
		wtestString5.PopBack();
		wtestString5.PopBack(); // TransformLargeToSmall() check;
		wssCopy.Clear(); // Clear() check;
		wlsCopy.Clear();
		//wempty3.PopBack();
		//wempty3.PopBack(); // Exception check;
		std::wcout << "WString: " << wlsSample.GetCString()
			<< "\nWString size: " << wlsSample.GetSize()
			<< "\nWString capacity: " << wlsSample.GetCapacity()
			<< "\nWString: " << wssSample.GetCString()
			<< "\nWString size: " << wssSample.GetSize()
			<< "\nWString capacity: " << wssSample.GetCapacity()
			<< "\nWString: " << wcharSample.GetCString()
			<< "\nWString size: " << wcharSample.GetSize()
			<< "\nWString capacity: " << wcharSample.GetCapacity()
			<< "\nWString: " << wempty1.GetCString()
			<< "\nWString size: " << wempty1.GetSize()
			<< "\nWString capacity: " << wempty1.GetCapacity() << "\n";
		std::sort(wtestString4.begin(), wtestString4.end()); // RAI check;
		std::wcout << wtestString4.GetCString();
	}
	catch (WString::OutOfRange e) {
		std::wcout << "Exception caught: " << e.what() << '\n';
	}
	return 0;
}

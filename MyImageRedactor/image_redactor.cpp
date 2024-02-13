#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <unordered_map>
#include <string>
#include <functional>

enum class Channel {R, G, B};


struct Pixel {
	uint8_t B = 0;
	uint8_t G = 0;
	uint8_t R = 0;
	uint8_t A = 255;

	uint8_t& GetChannel(const Channel ch) {
		switch (ch) {
		case Channel::B: return B; 
		case Channel::G: return G;
		case Channel::R: return R;
		}
	}
};


class Image {
public:
	virtual ~Image() = default;
	virtual void Read(std::ifstream& file) = 0;
	virtual void Write(std::ofstream& file) const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetWidth() const = 0;
	virtual Pixel& GetPixel(std::size_t y, std::size_t x) = 0;
};


class Filter {
public:
	virtual ~Filter() = default;
	virtual std::size_t GetArity() const = 0;
	virtual void SetArguments(std::size_t n, const std::string& arg) = 0;
	virtual void Apply(Image& image) const = 0;
};

#pragma pack(push, 1)

struct BMPFileHeader {
	uint16_t file_type = 0x4D42;          
	uint32_t file_size = 0;               
	uint16_t reserved1 = 0;              
	uint16_t reserved2 = 0;               
	uint32_t pixel_data = 0;          
};


struct BMPInfoHeader {
	uint32_t header_size = 0;                     
	int32_t width = 0;                      
	int32_t height = 0;                    
	uint16_t planes = 1;                   
	uint16_t bit_count = 0;                 
	uint32_t compression = 0;               
	uint32_t size_image = 0;                
	int32_t x_pixels_per_meter{ 0 };
	int32_t y_pixels_per_meter{ 0 };
	uint32_t colors_used{ 0 };              
	uint32_t colors_important{ 0 };    
};


struct BMPColorHeader {
	uint32_t red_mask = 0x00ff0000;         
	uint32_t green_mask = 0x0000ff00;       
	uint32_t blue_mask = 0x000000ff;        
	uint32_t alpha_mask = 0xff000000;       
	uint32_t color_space_type = 0x73524742; 
	uint32_t unused[16] = { 0 };                
};

#pragma pack(pop)

class BMP : public Image {
public:
	BMPFileHeader file_header;
	BMPInfoHeader info_header;
	BMPColorHeader color_header;
	std::vector<Pixel> pixel_data;

	void Read(std::ifstream& file) override {
		std::vector<uint8_t> temp_data;
		file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
		if (file_header.file_type != 0x4D42) {
			throw std::runtime_error("Error! Wrong file format");
		}
		file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));
		if (info_header.bit_count == 32) {
			if (info_header.header_size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
				file.read(reinterpret_cast<char*>(&color_header), sizeof(color_header));
				CheckColorHeader(color_header);
			} else {
				throw std::runtime_error("Error! Unrecognized file format: the file does not seem to contain bit mask information");
			}

		}
		file.seekg(file_header.pixel_data, file.beg);
		if (info_header.bit_count == 32) {
			info_header.header_size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
			file_header.pixel_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

		} else {
			info_header.header_size = sizeof(BMPInfoHeader);
			file_header.pixel_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
		}
		file_header.file_size = file_header.pixel_data;
		if (info_header.height < 0) { 
			throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
		}
		temp_data.resize(info_header.width * info_header.height * info_header.bit_count / 8); 
		if (info_header.width % 4 == 0) {
			file.read(reinterpret_cast<char*>(temp_data.data()), temp_data.size());
			file_header.file_size += temp_data.size();
		} else {
			row_stride = info_header.width * info_header.bit_count / 8;
			uint32_t new_stride = MakeStrideAligned(4, row_stride);
			std::vector<uint8_t> padding_row(new_stride - row_stride);
			for (int y = 0; y < info_header.height; ++y) {
				file.read(reinterpret_cast<char*>(temp_data.data() + row_stride * y), row_stride);
				file.read(reinterpret_cast<char*>(padding_row.data()), padding_row.size());
			}
			file_header.file_size += temp_data.size() + info_header.height * padding_row.size();
		}
		ConvertBitsToPixels(temp_data);
	}

	void Write(std::ofstream& file) const override {
		std::vector<uint8_t> temp_data;
		ConvertPixelsToBits(temp_data);
		if (info_header.bit_count == 32) {
			WriteFile(file, temp_data);
		} else if (info_header.bit_count == 24) {
			if (info_header.width % 4 == 0) {
				WriteFile(file, temp_data);
			} else {
				uint32_t new_stride = MakeStrideAligned(4, row_stride);
				std::vector<uint8_t> padding_row(new_stride - row_stride);
				WriteHeaders(file);
				for (int y = 0; y < info_header.height; ++y) {
					file.write(reinterpret_cast<const char*>(temp_data.data() + row_stride * y), row_stride);
					file.write(reinterpret_cast<const char*>(padding_row.data()), padding_row.size());
				}
			}
		} else {
			throw std::runtime_error("Error! Only 24 or 32 bits per pixel BMP files can be written");
		}
	}

	int GetHeight() const override {
		return info_header.height;
	}

	int GetWidth() const override {
		return info_header.width;
	}

	Pixel& GetPixel(std::size_t y, std::size_t x) override {
		return pixel_data[y * info_header.width + x];
	}

private:
	uint32_t row_stride = 0;

	static void CheckColorHeader(const BMPColorHeader& color_header) {
		BMPColorHeader color_header_ref;
		if (color_header_ref.red_mask != color_header.red_mask ||
			color_header_ref.green_mask != color_header.green_mask ||
			color_header_ref.blue_mask != color_header.blue_mask ||
			color_header_ref.alpha_mask != color_header.alpha_mask) {
			throw std::runtime_error("Error! Wrong color mask format");
		}
		if (color_header_ref.color_space_type != color_header.color_space_type) {
			throw std::runtime_error("Error! Wrong color space type");
		}
	}

	static uint32_t MakeStrideAligned(uint32_t align_stride, int32_t cur_stride) {
		return cur_stride + (align_stride - (cur_stride % align_stride)); 
	}

	void WriteHeaders(std::ofstream& file) const {
		file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
		file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
		if (info_header.bit_count == 32) {
			file.write(reinterpret_cast<const char*>(&color_header), sizeof(color_header));
		}
	}

	void WriteFile(std::ofstream& file, const std::vector<uint8_t>& data) const {
		WriteHeaders(file);
		file.write(reinterpret_cast<const char*>(data.data()), data.size());
	}

	void ConvertBitsToPixels(const std::vector<uint8_t>& bits) {
		uint32_t channels = info_header.bit_count / 8;
		for (std::size_t index = 0; index < bits.size(); index += channels) {
			Pixel pixel;
			pixel.B = bits[index];
			pixel.G = bits[index + 1];
			pixel.R = bits[index + 2];
			if (channels == 4) {
				pixel.A = bits[index + 3];
			}
			pixel_data.push_back(pixel);
		}
	}

	void ConvertPixelsToBits(std::vector<uint8_t>& bits) const {
		uint32_t channels = info_header.bit_count / 8;
		for (std::size_t index = 0; index < pixel_data.size(); index++) {
			bits.push_back(pixel_data[index].B);
			bits.push_back(pixel_data[index].G);
			bits.push_back(pixel_data[index].R);
			if (channels == 4) {
				bits.push_back(pixel_data[index].A);
			}
		}
	}
};


class DiscolorFilter : public Filter {
public:
	DiscolorFilter() = default;
	std::size_t GetArity() const override {
		return 0;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		return;
	}
	void Apply(Image& image) const override {
		discolor(image);
	}
private:
	static void discolor(Image& image) {
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				uint8_t color = image.GetPixel(y, x).B * 0.114 + image.GetPixel(y, x).G * 0.587 + image.GetPixel(y, x).R * 0.299;
				image.GetPixel(y, x).B = color;
				image.GetPixel(y, x).G = color;
				image.GetPixel(y, x).R = color;
			}
		}
	}
};


class LightenFilter : public Filter {
public:
	std::size_t GetArity() const override {
		return 1;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		scale = std::stoi(arg);
	}
	void Apply(Image& image) const override {
		lighten(image, scale);
	}
private:
	uint8_t scale = 0;
	static void lighten(Image& image, uint8_t scale) {
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				image.GetPixel(y, x).B = std::min(255, image.GetPixel(y, x).B + scale); 
				image.GetPixel(y, x).G = std::min(255, image.GetPixel(y, x).G + scale);
				image.GetPixel(y, x).R = std::min(255, image.GetPixel(y, x).R + scale);
			}
		}
	}
};


class DarkenFilter : public Filter {
public:
	std::size_t GetArity() const override {
		return 1;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		scale = std::stoi(arg);
	}
	void Apply(Image& image) const override {
		darken(image, scale);
	}
private:
	uint8_t scale = 0;
	static void darken(Image& image, uint8_t scale) {
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				image.GetPixel(y, x).B = std::max(0, image.GetPixel(y, x).B - scale); 
				image.GetPixel(y, x).G = std::max(0, image.GetPixel(y, x).G - scale);
				image.GetPixel(y, x).R = std::max(0, image.GetPixel(y, x).R - scale);
			}
		}
	}
};


class ContrastFilter : public Filter {
public:
	std::size_t GetArity() const override {
		return 0;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		return;
	}
	void Apply(Image& image) const override {
		contrast(image);
	}
private:
	static void contrast(Image& image) {
		uint8_t min_contrast = 255;
		uint8_t max_contrast = 0;
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				uint8_t color = image.GetPixel(y, x).B / 3 + image.GetPixel(y, x).G / 3 + image.GetPixel(y, x).R / 3;
				min_contrast = std::min(color, min_contrast); 
				max_contrast = std::max(color, max_contrast);
			}
		}
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				uint8_t color = image.GetPixel(y, x).B / 3 + image.GetPixel(y, x).G / 3 + image.GetPixel(y, x).R / 3;
				uint8_t new_color = 255 * (color - min_contrast) / (max_contrast - min_contrast);
				if (color == 0 || new_color == 0)
					continue;
				image.GetPixel(y, x).B = image.GetPixel(y, x).B * new_color / color;
				image.GetPixel(y, x).G = image.GetPixel(y, x).G * new_color / color;
				image.GetPixel(y, x).R = image.GetPixel(y, x).R * new_color / color;
			}
		}
	}
};


class ColorFilter : public Filter { 
public:
	std::size_t GetArity() const override {
		return 2;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		if (n == 0) {
			scale = std::stoi(arg);
		}
		else {
			filter = InterpretArg(arg);
		}
	}
	void Apply(Image& image) const override {
		Colorize(image, scale, filter);
	}
private:
	uint8_t scale = 0;
	Channel filter = Channel::B;
	static void Colorize(Image& image, uint8_t scale, Channel filter) {
		for (std::size_t y = 0; y < image.GetHeight(); y++) {
			for (std::size_t x = 0; x < image.GetWidth(); x++) {
				image.GetPixel(y, x).GetChannel(filter) = std::min(255, image.GetPixel(y, x).GetChannel(filter) + scale); // Naosareta
			}
		}
	}
	static Channel InterpretArg(std::string arg) {
		if (arg == "blue") {
			return Channel::B;
		}
		else if (arg == "red") {
			return Channel::R;
		}
		else if (arg == "green") {
			return Channel::G;
		}
		else {
			throw std::runtime_error("Error! Invalid argument!");
		}
	}
};


class BlurFilter : public Filter {
public:
	std::size_t GetArity() const override {
		return 1;
	}
	void SetArguments(std::size_t n, const std::string& arg) override {
		if (n >= GetArity()) {
			throw std::runtime_error("Error! Wromg number of arguments");
		}
		scale = std::stoi(arg);
	}
	void Apply(Image& image) const override {
		blur(image, scale);
	}
private:
	uint8_t scale = 0;
	void blur(Image& image, uint8_t scale) const {
		for (int y = 0; y < image.GetHeight(); y++) {
			for (int x = 0; x < image.GetWidth(); x++) {
				int neighbourBsum = 0;
				int neighboursB = 0;
				int neighbourGsum = 0;
				int neighboursG = 0;
				int neighbourRsum = 0;
				int neighboursR = 0;
				for (int ny = std::max(y - scale, 0); ny < std::min(image.GetHeight(), y + scale); ny++) {
					for (int nx = std::max(x - scale, 0); nx < std::min(image.GetWidth(), x + scale); nx++) {
						neighbourBsum += image.GetPixel(ny, nx).B; neighboursB++;
						neighbourGsum += image.GetPixel(ny, nx).G; neighboursG++;
						neighbourRsum += image.GetPixel(ny, nx).R; neighboursR++;
					}
				}
				image.GetPixel(y, x).B = neighbourBsum / neighboursB;
				image.GetPixel(y, x).G = neighbourGsum / neighboursG;
				image.GetPixel(y, x).R = neighbourRsum / neighboursR;
			}
		}
	}
};

int main(int argc, char** argv) {
	std::unordered_map<std::string, std::function<std::unique_ptr<Filter>()>> filters // Naosareta
	{
		{ "discolor", []() -> std::unique_ptr<Filter> { return std::make_unique<DiscolorFilter>(); } },
		{ "lighten", []() -> std::unique_ptr<Filter> { return std::make_unique<LightenFilter>(); } },
		{ "darken", []() -> std::unique_ptr<Filter> { return std::make_unique<DarkenFilter>(); } },
		{ "colorize", []() -> std::unique_ptr<Filter> { return std::make_unique<ColorFilter>(); } },
		{ "contrast", []() -> std::unique_ptr<Filter> { return std::make_unique<ContrastFilter>(); } },
		{ "blur", []() -> std::unique_ptr<Filter> { return std::make_unique<BlurFilter>(); } }
	};
	BMP bmp;
	std::ifstream fin;
	int arg_pos = 1;
	std::string first_arg = argv[arg_pos];
	arg_pos++;
	if (argc == 2 && first_arg == "help") {
		std::cerr << "Welcome to image redactor! At a certain moment it is possible to work only with bmp images. Available commands:" <<
			"\ndiscolor;\nlighten -integer-;\ndarken -integer-;\nblue -integer-;\nred -integer-;\ngreen -integer-;\ncontrast;\nblur -integer-;\nhelp;" <<
			"\nAvailable command formats :\nimage_redactor.exe file_to_read_and_write command (optional)integer;\nimage_redactor.exe file_to_read command (optional)integer file_to_write;;\nimage_redactor.exe file_to_read colorize integer blue/red/green (optional)file_to_write;\nimage_redactor.exe help;";
		return -1;
	}
	fin.open(first_arg, std::ios::binary);
	if (fin) {
		bmp.Read(fin);
	} else {
		throw std::runtime_error("Error! Unable to open the file");
	}
	fin.close();
	std::string command = argv[arg_pos];
	arg_pos++;
	std::unique_ptr<Filter> filter = filters[command]();
	for (std::size_t arg_count = 0; arg_count < filter->GetArity(); arg_count++) {
		std::string val = argv[arg_pos];
		arg_pos++;
		filter->SetArguments(arg_count, val);
	}
	filter->Apply(bmp);
	std::ofstream fout;
	if (arg_pos == argc) {
		fout.open(first_arg, std::ios::binary);
	} else {
		std::string file = argv[arg_pos];
		fout.open(file, std::ios::binary);
	}
	if (fout) {
		bmp.Write(fout);
	} else {
		throw std::runtime_error("Error! Unable to open the file");
	}
	return 0;
}	
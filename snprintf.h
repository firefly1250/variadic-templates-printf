namespace Zsnprintf {
	constexpr size_t MAX_NUMBER_LENGTH = 20;

	struct Pad {
		bool right = true;
		bool zero = false;
		bool plus = false;

		static Pad Default() {
			Pad pad;
			return pad;
		}
	};

	size_t Zsnprintf(char* buf, const size_t max_width, const char* format);

	template<typename Head, typename ... Tail>
	size_t Zsnprintf(char* buf, const size_t max_width, const char* format, const Head& head, const Tail&... tail);

	size_t PrintChar(char* &buf,const char c);

	//char
	size_t Output(char* &buf,const char c, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad);
	
	//bool
	size_t Output(char* &buf,const bool b, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad);

	//string
	size_t Output(char* &buf,const char* str, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad);		size_t i = 0;
	//int
	size_t Output(char* &buf,const int _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad);

	//float
	size_t Output(char* &buf,const float _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad);

	size_t GetWidth(const char* str);
	size_t GetWidth(const int a);
    
	float Pow10(const unsigned int n);
	template<typename T>
	T Abs(T n);
}
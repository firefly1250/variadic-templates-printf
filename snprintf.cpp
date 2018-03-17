#include <stdio.h>

class Snprintf {
	static constexpr size_t MAX_NUMBER_LENGTH = 20;
	unsigned char buffer[256];
	unsigned char itr = 0;

	struct Pad {
		bool right = true;
		bool zero = false;
		bool plus = false;

		static Pad Default() {
			Pad pad;
			return pad;
		}
	};

public:
	Snprintf() {
		for (auto &e : buffer) e = '\0';
	}

	size_t Print(const char* format, const size_t max_width) {
		if (format == nullptr) return 0;
		return Output(format, max_width, 0, 0, Pad::Default());
	}

	template<typename Head, typename ... Tail>
	size_t Print(const char* format, const size_t max_width, const Head& head, const Tail&... tail) {
		if (format == nullptr) return 0;

		size_t printed_width = 0;
		for (; *format != '%'; format++) {
			if (max_width == printed_width || *format == '\0') return printed_width;
			printed_width += PrintChar(*format);
		}
		format++;
		size_t width = 0;
		Pad pad;

		if (*format == '\0') return printed_width;
		if (*format == '%') {
			printed_width += PrintChar('%');
			return printed_width + Print(++format, max_width - printed_width, head, tail...);
		}
		for (; *format == '-' || *format == '+'; format++) {
			if (*format == '-') pad.right = false;
			if (*format == '+') pad.plus = true;
		}
		for (; *format == '0'; format++) pad.zero = true;
		for (; *format >= '0' && *format <= '9'; format++) {
			width *= 10;
			width += *format - '0';
		}
		size_t width_decimal;
		if (*format == '.'){
			width_decimal=0;
			format++;
			for (; *format >= '0' && *format <= '9'; format++) {
				width_decimal *= 10;
				width_decimal += *format - '0';
			}
		}
		else width_decimal = 6;

		printed_width += Output(head, max_width, width, width_decimal, pad);
		return printed_width + Print(++format, max_width - printed_width, tail...);
	}

	void Show() {
		printf("%s", buffer);
	}

private:
	size_t PrintChar(const char c) {
		buffer[itr++] = c;
		return 1;
	}

	//char
	size_t Output(const char c, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		const char str[2] = { c,'\0' };
		return Output(str, max_width, width, width_decimal, pad);
	}
	
	//bool
	size_t Output(const bool b, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		return Output(b ? "true" : "false", max_width, width, width_decimal, pad);
	}

	//string
	size_t Output(const char* str, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		size_t i = 0;
		if (pad.right) {
			//if misses the cast, underflow can happen
			for (; (int)i < (int)width - (int)GetWidth(str); i++) {
				if (i == max_width) return max_width;
				if (pad.zero) buffer[itr++] = '0';
				else buffer[itr++] = ' ';
			}
		}
		for (; *str != '\0'; str++) {
			if (i == max_width) return max_width;
			buffer[itr++] = *str;
			i++;
		}
		if (!pad.right) {
			for (; i < width; i++) {
				if (i == max_width) return max_width;
				buffer[itr++] = ' ';
			}
		}
		return i;
	}

	//int
	size_t Output(const int _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int integer = Abs(_n);
		const size_t width_integer = GetWidth(integer);
		size_t width_sign = 0;

		size_t printed_width = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_width += PrintChar('-');
			else {
				str[0] = '-';
				width_sign++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_width += PrintChar('+');
			else {
				str[0] = '+';
				width_sign++;
			}
		}

		for (int i = 0; i < width_integer ; i++) {
			str[width_sign + width_integer - i - 1] = integer % 10 + '0';
			integer /= 10;
		}
		str[width_sign + width_integer] = '\0';
		return printed_width + Output(str, max_width - printed_width, width - printed_width, width_decimal, pad);
	}

	//float
	size_t Output(const float _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int integer = (int)Abs(_n);
		int decimal = (Abs(_n) - (float)integer)*Pow10(width_decimal);
		const size_t width_integer = GetWidth(integer);
		size_t width_sign = 0;

		size_t printed_width = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_width += PrintChar('-');
			else {
				str[0] = '-';
				width_sign++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_width += PrintChar('+');
			else {
				str[0] = '+';
				width_sign++;
			}
		}

		for (int i = 0; i < width_integer ; i++) {
			str[width_sign + width_integer - i - 1] = integer % 10 + '0';
			integer /= 10;
		}
		if(width_decimal > 0){
			str[width_sign + width_integer] = '.';
			for (int i = 0; i < width_decimal ; i++) {
				str[width_sign + width_integer + width_decimal - i] = decimal % 10 + '0';
				decimal /= 10;
			}	
			str[width_sign + width_integer + width_decimal + 1] = '\0';
		}
		else str[width_sign + width_integer] = '\0';
		
		return printed_width + Output(str, max_width - printed_width, width - printed_width, width_decimal, pad);
	}

	size_t GetWidth(const char* str) {
		size_t i = 0;
		for (; str[i] != '\0'; i++);
		return i;
	}

	size_t GetWidth(const int a) {
		int n = Abs(a);
		if (n == 0) return 1;
		size_t i = 0;
		for (; n > 0; i++) n /= 10;
		return i;
	}

	float Pow10(const unsigned int n){
		float p = 1;
		for(unsigned int i=0;i<n;i++) p*=10;
		return p;
	}

	template<typename T>
	T Abs(T n) {
		if (n >= 0) return n;
		else return -n;
	}


};



int main()
{
	Snprintf po;
	auto n = po.Print("_%6.5z_%-3z_%04z_%7.3z_", 100, "poyo", 'x', 456, 5.3f);
	po.Show();
	printf("\nlength = %d\n", n);

	return 0;
}
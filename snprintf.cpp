#include <stdio.h>

using int32_t = int;
using uint32_t = unsigned int;

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

	size_t Print(const char* format, const size_t max_length) {
		if (format == nullptr) return 0;
		return Output(format, max_length, 0, 0, Pad::Default());
	}

	template<typename Head, typename ... Tail>
	size_t Print(const char* format, const size_t max_length, const Head& head, const Tail&... tail) {
		if (format == nullptr) return 0;

		size_t length = 0;
		for (; *format != '%'; format++) {
			if (max_length == length || *format == '\0') return length;
			length += PrintChar(*format);
		}
		format++;
		size_t width = 0;
		Pad pad;

		if (*format == '\0') return length;
		if (*format == '%') {
			length += PrintChar('%');
			return length + Print(++format, max_length - length, head, tail...);
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

		length += Output(head, max_length, width, width_decimal, pad);
		return length + Print(++format, max_length - length, tail...);
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
	size_t Output(const char c, const size_t max_length, const size_t width,const size_t width_decimal,const Pad pad) {
		const char str[2] = { c,'\0' };
		return Output(str, max_length, width, width_decimal, pad);
	}
	
	//bool
	size_t Output(const bool b, const size_t max_length, const size_t width,const size_t width_decimal,const Pad pad) {
		return Output(b ? "true" : "false", max_length, width, width_decimal, pad);
	}

	//string
	size_t Output(const char* str, const size_t max_length, const size_t width,const size_t width_decimal,const Pad pad) {
		const size_t length = GetLength(str);
		size_t i = 0;
		if (pad.right) {
			//if misses the cast, underflow can happen
			for (; (int32_t)i < (int32_t)(width - length); i++) {
				if (i == max_length) return max_length;
				if (pad.zero) buffer[itr++] = '0';
				else buffer[itr++] = ' ';
			}
		}
		for (; *str != '\0'; str++) {
			if (i == max_length) return max_length;
			buffer[itr++] = *str;
			i++;
		}
		if (!pad.right) {
			for (; i < width; i++) {
				if (i == max_length) return max_length;
				buffer[itr++] = ' ';
			}
		}
		return i;
	}

	//int
	size_t Output(const int32_t _n, const size_t max_length, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int32_t n = Abs(_n);
		const size_t digit = GetDigit(n);
		size_t length_to_print = digit;

		size_t printed_length = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_length += PrintChar('-');
			else {
				str[0] = '-';
				length_to_print++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_length += PrintChar('+');
			else {
				str[0] = '+';
				length_to_print++;
			}
		}

		for (int32_t i = 0; i < digit ; i++) {
			str[length_to_print - i - 1] = n % 10 + '0';
			n /= 10;
		}
		str[length_to_print] = '\0';
		return printed_length + Output(str, max_length - printed_length, width - printed_length, width_decimal, pad);
	}

	//float
	size_t Output(const float _n, const size_t max_length, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int32_t integer = (int32_t)Abs(_n);
		int32_t decimal = (Abs(_n) - (float)integer)*Pow10(width_decimal);
		const size_t length_integer = GetDigit(integer);
		size_t length_sign = 0;

		size_t printed_length = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_length += PrintChar('-');
			else {
				str[0] = '-';
				length_sign++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_length += PrintChar('+');
			else {
				str[0] = '+';
				length_sign++;
			}
		}

		for (int32_t i = 0; i < length_integer ; i++) {
			str[length_sign + length_integer - i - 1] = integer % 10 + '0';
			integer /= 10;
		}
		if(width_decimal > 0){
			str[length_sign + length_integer] = '.';
			for (int32_t i = 0; i < width_decimal ; i++) {
				str[length_sign + length_integer + width_decimal - i] = decimal % 10 + '0';
				decimal /= 10;
			}	
			str[length_sign + length_integer + width_decimal + 1] = '\0';
		}
		else str[length_sign + length_integer] = '\0';
		
		return printed_length + Output(str, max_length - printed_length, width - printed_length, width_decimal, pad);
	}

	size_t GetLength(const char* str) {
		size_t i = 0;
		for (; str[i] != '\0'; i++);
		return i;
	}


	size_t GetDigit(const int32_t a) {
		int32_t n = Abs(a);
		if (n == 0) return 1;
		size_t i = 0;
		for (; n > 0; i++) n /= 10;
		return i;
	}

	float Pow10(const uint32_t n){
		float p = 1;
		for(uint32_t i=0;i<n;i++) p*=10;
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
	auto n = po.Print("_%6.5z_%-3z_%z_%z_", 100, "poyo", 'x', 12, -1.23456789f);
	po.Show();
	printf("\nlength = %d\n", n);

	return 0;
}
#include <stdio.h>
#include "snprintf.h"


	size_t Zsnprintf::Zsnprintf(char* buf, const size_t max_width, const char* format) {
		if (format == nullptr) return 0;
		return Output(buf, format, max_width, 0, 0, Pad::Default());
	}

	template<typename Head, typename ... Tail>
	size_t Zsnprintf::Zsnprintf(char* buf, const size_t max_width, const char* format, const Head& head, const Tail&... tail) {
		if (format == nullptr) return 0;

		size_t printed_width = 0;
		for (; *format != '%'; format++) {
			if (max_width == printed_width || *format == '\0') return printed_width;
			printed_width += PrintChar(buf, *format);
		}
		format++;
		size_t width = 0;
		Pad pad;

		if (*format == '\0') return printed_width;
		if (*format == '%') {
			printed_width += PrintChar(buf, '%');
			return printed_width + Zsnprintf(buf, max_width - printed_width, ++format, head, tail...);
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

		printed_width += Output(buf, head, max_width - printed_width, width, width_decimal, pad);
		return printed_width + Zsnprintf(buf,  max_width - printed_width, ++format, tail...);
	}

	size_t Zsnprintf::PrintChar(char* &buf,const char c) {
		*buf++ = c;
		return 1;
	}

	//char
	size_t Zsnprintf::Output(char* &buf,const char c, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		const char str[2] = { c,'\0' };
		return Output(buf, str, max_width, width, width_decimal, pad);
	}
	
	//bool
	size_t Zsnprintf::Output(char* &buf,const bool b, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		return Output(buf, b ? "true" : "false", max_width, width, width_decimal, pad);
	}

	//string
	size_t Zsnprintf::Output(char* &buf,const char* str, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		size_t i = 0;
		if (pad.right) {
			//if misses the cast, underflow can happen
			for (; (int)i < (int)width - (int)GetWidth(str); i++) {
				if (i == max_width) return max_width;
				if (pad.zero) *buf++ = '0';
				else *buf++ = ' ';
			}
		}
		for (; *str != '\0'; str++) {
			if (i == max_width) return max_width;
			*buf++ = *str;
			i++;
		}
		if (!pad.right) {
			for (; i < width; i++) {
				if (i == max_width) return max_width;
				*buf++ = ' ';
			}
		}
		return i;
	}

	//int
	size_t Zsnprintf::Output(char* &buf,const int _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int integer = Abs(_n);
		const size_t width_integer = GetWidth(integer);
		size_t width_sign = 0;

		size_t printed_width = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_width += PrintChar(buf, '-');
			else {
				str[0] = '-';
				width_sign++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_width += PrintChar(buf, '+');
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
		return printed_width + Output(buf, str, max_width - printed_width, width - printed_width, width_decimal, pad);
	}

	//float
	size_t Zsnprintf::Output(char* &buf,const float _n, const size_t max_width, const size_t width,const size_t width_decimal,const Pad pad) {
		char str[MAX_NUMBER_LENGTH];

		int integer = (int)Abs(_n);
		int decimal = (Abs(_n) - (float)integer)*Pow10(width_decimal);
		const size_t width_integer = GetWidth(integer);
		size_t width_sign = 0;

		size_t printed_width = 0;
		if (_n < 0) {
			if (pad.zero && pad.right) printed_width += PrintChar(buf, '-');
			else {
				str[0] = '-';
				width_sign++;
			}
		}
		if (_n >= 0 && pad.plus) {
			if (pad.zero && pad.right) printed_width += PrintChar(buf, '+');
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
		
		return printed_width + Output(buf, str, max_width - printed_width, width - printed_width, width_decimal, pad);
	}

	size_t Zsnprintf::GetWidth(const char* str) {
		size_t i = 0;
		for (; str[i] != '\0'; i++);
		return i;
	}

	size_t Zsnprintf::GetWidth(const int a) {
		int n = Abs(a);
		if (n == 0) return 1;
		size_t i = 0;
		for (; n > 0; i++) n /= 10;
		return i;
	}

	float Zsnprintf::Pow10(const unsigned int n){
		float p = 1;
		for(unsigned int i=0;i<n;i++) p*=10;
		return p;
	}

	template<typename T>
	T Zsnprintf::Abs(T n) {
		if (n >= 0) return n;
		else return -n;
	}



int main()
{
	char buffer[256]{};
	char* po = buffer;
	auto n = Zsnprintf::Zsnprintf(buffer, 100, "_%6.5z_%-3z_%04z_%7.3z_", "poyo", 'x', 456, 5.3f);
	//auto n = Zsnprintf::Zsnprintf(po, 100, "_1234567890_");
	//auto n = po.Print("_%z_", 3, "poyo");
	printf("%s\n", buffer);
	printf("length = %d\n", n);

	return 0;
}
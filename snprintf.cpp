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

	int Print(const char* format, const size_t max_length) {
		if (format == nullptr) return 0;
		return Output(format, max_length, 0, Pad::Default());
	}

	template<typename Head, typename ... Tail>
	int Print(const char* format, const size_t max_length, const Head& head, const Tail&... tail) {
		if (format == nullptr) return 0;

		size_t length = 0;
		for (; *format != '%'; format++) {
			if (max_length == length || *format == '\0') return length;
			length += PrintChar(*format);
		}
		format++;
		int width = 0;
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

		length += Output(head, max_length, width, pad);
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

	size_t Output(const char c, const size_t max_length, const size_t width, const Pad pad) {
		const char str[2] = { c,'\0' };
		return Output(str, max_length, width, pad);
	}

	size_t Output(const char* str, const size_t max_length, const size_t width, const Pad pad) {
		const size_t length = GetLength(str);
		size_t i = 0;
		if (pad.right) {
			//if misses the cast, underflow can happen
			for (; (int)i < (int)(width - length); i++) {
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
	size_t Output(const int _n, const size_t max_length, const size_t width, const Pad pad) {
		char str[MAX_NUMBER_LENGTH]{};

		int n = Abs(_n);
		size_t length_to_print = GetOrder(n);

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

		for (size_t i = length_to_print - 1; n > 0; i--) {
			str[i] = n % 10 + '0';
			n /= 10;
		}
		return printed_length + Output(str, max_length - printed_length, width - printed_length, pad);
	}

	size_t GetLength(const char* str) {
		size_t i = 0;
		for (; str[i] != '\0'; i++);
		return i;
	}


	size_t GetOrder(const int a) {
		int n = Abs(a);
		if (n == 0) return 1;
		size_t i = 0;
		for (; n > 0; i++) n /= 10;
		return i;
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
	printf("%d\n", po.Print("_%6z_%-3z_%+z_", 100, "poyo", 'x', 23));
	po.Show();
	printf("\n");

	return 0;
}
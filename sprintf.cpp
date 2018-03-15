class Output {
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
	Output() {
		for (auto &e : buffer) e = '\0';
	}

	int Print(const char* format, const size_t max_length) {
		if (format == nullptr || *format == '\0' || max_length == 0) return 0;
		return  PrintString(format, max_length, 0, Pad::Default());
	}

	template<typename Head, typename ... Tail>
	int Print(const char* format, const size_t max_length, const Head& head, const Tail&... tail) {
		if (format == nullptr || *format == '\0' || max_length == 0) return 0;

		size_t length = 0;

		if (*format == '%') {
			format++;
			int width = 0;
			Pad pad;

			if (*format == '\0') return 0;
			if (*format == '%') {
				length = PrintChar('%');
				return length + Print(++format, max_length - length, tail...);
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

			switch (*format) {
			case 'd':
				length = PrintInt((const int32_t)head, max_length, width, pad);
				break;
			case 'u':
				length = PrintInt((const uint32_t)head, max_length, width, pad);
				break;
			case 's':
				length = PrintString((const char *)head, max_length, width, pad);
				break;
			case 'c':
				const char str[2] = { (char)head,'\0' };
				length = PrintString(str, max_length, width, pad);
				break;
			}

			return length + Print(++format, max_length - length, tail...);
		}
		else {
			length = PrintChar((const char)*format);
			return length + Print(++format, max_length - length, head, tail...);
		}
		return 0;
	}

	size_t PrintChar(const char c) {
		buffer[itr++] = c;
		return 1;
	}

	size_t PrintString(const char* str, const size_t max_length, const size_t width, const Pad pad) {
		const size_t length = GetLength(str);
		size_t i = 0;
		if (pad.right) {
			//if misses the cast, underflow can happen when width < length
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
	size_t PrintInt(const int _n, const size_t max_length, const size_t width, const Pad pad) {
		char str[MAX_NUMBER_LENGTH]{};
		//for (auto& e : str) e = '\0';

		int n = abs(_n);
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
		return printed_length + PrintString(str, max_length - printed_length, width - printed_length, pad);
	}

	size_t GetLength(const char* str) {
		size_t i = 0;
		for (; str[i] != '\0'; i++);
		return i;
	}


	int GetOrder(const int a) {
		int n = abs(a);
		if (n == 0) return 1;
		size_t i = 0;
		for (; n > 0; i++) n /= 10;
		return i;
	}
};
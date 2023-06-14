#ifndef GODUI_UNIT_H
#define GODUI_UNIT_H

struct Unit {
	enum UnitType {
		INVALID = 0,
		PIXELS = 1,
		PERCENTAGE = 2
	};
	
	UnitType type;
	float value;

	inline Unit(): type(INVALID), value(0.0) {}
	inline Unit(float p_value): type(PIXELS), value(p_value) {}
	inline Unit(UnitType p_type, float p_value): type(p_type), value(p_value) {}
	inline Unit(const char32_t *p_unit) {
		type = PIXELS;
		value = 0.0;

		uint64_t int_part = 0;
		uint64_t dec_part = 0;
		float dec_mul = 1.0;
		bool negative = false;
		bool decimal = false;
		bool white = false;
		bool found = false;

		#define INVALIDATE(p_condition) if (p_condition) {type = INVALID; break;}

		while (*p_unit != '\0') {
			if (*p_unit == '-') {
				// Must be the first digit
				INVALIDATE(int_part > 0 || decimal || negative)
				negative = true;
				white = false;
			} else if (*p_unit >= '0' && *p_unit <= '9') {
				// Disallow any digit after suffix
				INVALIDATE(found)

				// Can't start with 0
				INVALIDATE(!decimal && *p_unit == '0' && int_part == 0)

				// Disallow white space between digits
				INVALIDATE((int_part > 0 || dec_mul < 1.0) && white)

				// Currently extracting decimal portion (after '.')
				if (decimal) {
					dec_part *= 10;
					dec_part += (*p_unit - '0');
					dec_mul *= 0.1;
				// Currently extracting integer portion (before '.')
				} else {
					int_part *= 10;
					int_part += (*p_unit - '0');
				}
			} else if (*p_unit == '.') {
				// Disallow any digit after suffix
				INVALIDATE(found)

				// Dissallow '..'
				INVALIDATE(decimal)

				// Dissallow ' .'
				decimal = true;
			} else if (*p_unit == ' ') {
				white = true;
			} else if (*p_unit == '%') {
				// Disallow any digit after suffix
				INVALIDATE(found)

				type = PERCENTAGE;
				found = true;
			} else if (*p_unit == 'p') {
				// Disallow any digit after suffix
				INVALIDATE(found)

				// Must have a 'x' right after
				INVALIDATE(*(++p_unit) != 'x')

				type = PIXELS;
				found = true;
			} else INVALIDATE(true);
			p_unit++;
		}

		#undef INVALIDATE

		if (found) {
			value = ((float)int_part + ((float)dec_part) * dec_mul) * 0.01;
			if (negative) value *= -1;
		}
	}

	inline bool operator==(const Unit &p_other) const {
		return this->type == p_other.type && this->type == p_other.type;
	}

	inline bool operator!=(const Unit &p_other) const {
		return *this != p_other;
	}

	inline Unit &operator=(const Unit &p_other) {
		this->type = p_other.type;
		this->value = p_other.value;
		return *this;
	}
};

#endif // GODUI_UNIT_H
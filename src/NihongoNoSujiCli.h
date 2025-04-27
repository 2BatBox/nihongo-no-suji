#pragma once

#include "AppCli.h"

#include <cstdint>
#include <string>

struct NihongoNoSujiCli {

	enum EnumMethod : unsigned {
		DIGITS,
		NUMBERS,
		__SIZE
	};

	struct EnumMethodToCStr {
		static const char* to_cstr(const EnumMethod& value) {
			switch(value) {
				case EnumMethod::DIGITS: return "digits";
				case EnumMethod::NUMBERS: return "numbers";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Method = EnumField<EnumMethod, EnumMethodToCStr>;

	enum class EnumInputType : unsigned {
		ARABIC,
		HIRAGANA,
		KANJI,
		AUDIO,
		__SIZE
	};

	struct EnumInputTypeCStr {
		static const char* to_cstr(const EnumInputType& value) {
			switch(value) {
				case EnumInputType::ARABIC: return "arabic";
				case EnumInputType::HIRAGANA: return "hiragana";
				case EnumInputType::KANJI: return "kanji";
				case EnumInputType::AUDIO: return "audio";
				default: return "[UNKNOWN]";
			}
		}
	};

	using InputType = EnumField<EnumInputType, EnumInputTypeCStr>;

	enum class EnumOutputType : unsigned {
		ARABIC,
		HIRAGANA,
		__SIZE
	};

	struct EnumOutputTypeCStr {
		static const char* to_cstr(const EnumOutputType& value) {
			switch(value) {
				case EnumOutputType::ARABIC: return "arabic";
				case EnumOutputType::HIRAGANA: return "hiragana";
				default: return "[UNKNOWN]";
			}
		}
	};

	using OutputType = EnumField<EnumOutputType, EnumOutputTypeCStr>;

	CliOption<unsigned> rounds = CliOption<unsigned>('r', "Rounds.");
	CliOption<unsigned> digits_from = CliOption<unsigned>('f', "Digits from. (max 9 for numbers mode)");
	CliOption<unsigned> digits_to = CliOption<unsigned>('t', "Digits to. (max 9 for numbers mode)");
	CliOption<InputType> input_type = CliOption<InputType>('i', InputType::description().c_str());
	CliOption<OutputType> output_type = CliOption<OutputType>('o', InputType::description().c_str());


	AppCli<Method> action;

	NihongoNoSujiCli() {
		action[EnumMethod::DIGITS]
			.description("Digit sequences like phone numbers. (7425 -> ななよんにご).")
			.mandatory(rounds, digits_from, digits_to, input_type, output_type);

		action[EnumMethod::NUMBERS]
			.description("Numbers. (7425 -> ななせんよんひゃくにじゅうご).")
			.mandatory(rounds, digits_from, digits_to, input_type, output_type);

		action.finilize();
	}

	bool parse_args(int argc, char** argv) {
		return action.parse_args(argc, argv) && validate();
	}

	bool validate() const {
		bool result = true;
		result = result && digits_from.value > 0;
		result = result && digits_from.value <= digits_to.value;
		result = result && (action.method.value != EnumMethod::NUMBERS || digits_to.value < 10u);
		return result;
	}

	void print_usage(FILE* out) {
		action.print_usage(out);
	}

	std::string options_string() const {
		return action.options_string();
	}

};

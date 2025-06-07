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


	enum class EnumQuestion : unsigned {
		ARABIC,
		HIRAGANA,
		KANJI,
		AUDIO,
		__SIZE
	};

	struct EnumQuestionCStr {
		static const char* to_cstr(const EnumQuestion& value) {
			switch(value) {
				case EnumQuestion::ARABIC: return "arabic";
				case EnumQuestion::HIRAGANA: return "hiragana";
				case EnumQuestion::KANJI: return "kanji";
				case EnumQuestion::AUDIO: return "audio";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Question = EnumField<EnumQuestion, EnumQuestionCStr>;

	enum class EnumAnswer : unsigned {
		ARABIC,
		HIRAGANA,
		KANJI,
		__SIZE
	};

	struct EnumAnswerCStr {
		static const char* to_cstr(const EnumAnswer& value) {
			switch(value) {
				case EnumAnswer::ARABIC: return "arabic";
				case EnumAnswer::HIRAGANA: return "hiragana";
				case EnumAnswer::KANJI: return "kanji";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Answer = EnumField<EnumAnswer, EnumAnswerCStr>;

	unsigned pr = 1;
	Option<unsigned> rounds = Option<unsigned>('r', "Rounds.", ++pr);
	Option<unsigned> digits_from = Option<unsigned>('f', "Digits from. (max 9 for numbers mode)", ++pr);
	Option<unsigned> digits_to = Option<unsigned>('t', "Digits to. (max 9 for numbers mode)", ++pr);
	Option<Question> question = Option<Question>('q', Question::description(), ++pr);
	Option<Answer> answer = Option<Answer>('a', Answer::description(), ++pr);

	AppCliMethod<Method> action;

	NihongoNoSujiCli() {
		action[EnumMethod::DIGITS]
			.desc("Digit sequences like phone numbers. (7425 -> ななよんにご).")
			.mand(rounds, digits_from, digits_to, question, answer);

		action[EnumMethod::NUMBERS]
			.desc("Numbers. (7425 -> ななせんよんひゃくにじゅうご).")
			.mand(rounds, digits_from, digits_to, question, answer);

		action.finalize();
	}

	bool parse_args(int argc, char** argv) {
		return action.parse_args(argc, argv) && validate();
	}

	bool validate() const {
		bool result = true;
		result = result && digits_from.value() > 0;
		result = result && digits_from.value() <= digits_to.value();
		result = result && (action.action().value != EnumMethod::NUMBERS || digits_to.value() < 10u);
		return result;
	}

	void print_usage(FILE* out, const char* bin) {
		action.print_usage(out, bin);
	}

	std::string options_string() const {
		return action.options_string();
	}

};

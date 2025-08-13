#pragma once

#include "AppCli.h"

#include <cstdint>
#include <string>

struct NihongoNoSujiCli {

	enum EnumMethod : unsigned {
		LEARN,
		TEST,
		__SIZE
	};

	struct EnumMethodToCStr {
		static const char* to_cstr(const EnumMethod& value) {
			switch(value) {
				case EnumMethod::LEARN: return "learn";
				case EnumMethod::TEST: return "test";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Method = EnumField<EnumMethod, EnumMethodToCStr>;

	enum class EnumMode : unsigned {
		DIGITS,
		NUMBERS,
		TIME,
		__SIZE
	};

	struct EnumModeToCStr {
		static const char* to_cstr(const EnumMode& value) {
			switch(value) {
				case EnumMode::DIGITS: return "digits";
				case EnumMode::NUMBERS: return "numbers";
				case EnumMode::TIME: return "time";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Mode = EnumField<EnumMode, EnumModeToCStr>;

	unsigned pr = 1;
	Option<Mode> mode = Option<Mode>('M', Mode::description(), ++pr);
	Option<unsigned> rounds = Option<unsigned>('r', "Rounds.", ++pr);
	Option<unsigned> digits_from = Option<unsigned>('f', "Digits from. (max 9 for numbers mode)", ++pr);
	Option<unsigned> digits_to = Option<unsigned>('t', "Digits to. (max 9 for numbers mode)", ++pr);

	OptionFlag show_kanji_before = OptionFlag('j', "Show kanji before.", ++pr);
	OptionFlag show_kanji_after = OptionFlag('J', "Show kanji after.", ++pr);

	OptionFlag show_kana_before = OptionFlag('k', "Show kana before.", ++pr);
	OptionFlag show_kana_after = OptionFlag('K', "Show kana after.", ++pr);

	OptionFlag show_arabic_before = OptionFlag('a', "Show arabic before.", ++pr);
	OptionFlag show_arabic_after = OptionFlag('A', "Show arabic after.", ++pr);

	OptionFlag play_audio_before = OptionFlag('p', "Play audio before.", ++pr);
	OptionFlag play_audio_after = OptionFlag('P', "Play audio after.", ++pr);

	OptionFlag wait_for_user = OptionFlag('w', "Wait for user before the next question.", ++pr);

	AppCliMethod<Method> action;

	NihongoNoSujiCli() {
		action[EnumMethod::LEARN]
			.desc("Learning.")
			.mand(mode, rounds, digits_from, digits_to)
			.opt(
				show_kanji_before,
				show_kanji_after,
				show_kana_before,
				show_kana_after,
				show_arabic_before,
				show_arabic_after,
				play_audio_before,
				play_audio_after,
				wait_for_user
			);

		action[EnumMethod::TEST]
			.desc("Testing.")
			.mand(mode, rounds, digits_from, digits_to)
			.opt(
				show_kanji_before,
				show_kanji_after,
				show_kana_before,
				show_kana_after,
				show_arabic_before,
				show_arabic_after,
				play_audio_before,
				play_audio_after,
				wait_for_user
			);

		action.finalize();
	}

	bool parse_args(int argc, char** argv) {
		return action.parse_args(argc, argv) && validate();
	}

	bool validate() const {
		bool result = true;
		result = result && digits_from.value() > 0;
		result = result && digits_from.value() <= digits_to.value();
		result = result && (show_kanji_before.presented() || show_kana_before.presented() || show_arabic_before.presented() || play_audio_before.presented());
		return result;
	}

	void print_usage(FILE* out, const char* bin) {
		action.print_usage(out, bin);
	}

	std::string options_string() const {
		return action.options_string();
	}

};

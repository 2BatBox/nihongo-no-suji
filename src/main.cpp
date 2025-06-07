#include "NihongoNoSujiCli.h"
#include "DiceMachine.h"
#include "TermColor.h"

#include <cstdio>
#include <vector>
#include <locale>
#include <codecvt>

class NihongoNoSuji {

	using String_t = std::u32string;

	static constexpr const char32_t* DIGIT_MAP_ARABIC_SEP[] = {U"0 ", U"1 ", U"2 ", U"3 ", U"4 ", U"5 ", U"6 ", U"7 ", U"8 ", U"9 "};
	static constexpr const char32_t* DIGIT_MAP_ARABIC[] = {U"0", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8", U"9"};
	static constexpr const char32_t* DIGIT_MAP_HIRAGANA[] = {U"れい", U"いち", U"に", U"さん", U"よん", U"ご", U"ろく", U"なな", U"はち", U"きゅう"};
	static constexpr const char32_t* DIGIT_MAP_KANJI[] = {U"0", U"一", U"二", U"三", U"四", U"五", U"六", U"七", U"八", U"九"};

	using Buffer_t = std::vector<unsigned char>;

	const NihongoNoSujiCli _cli;
	DiceMachine _dm;

public:
	NihongoNoSuji(const NihongoNoSujiCli& cli) :
		_cli(cli), _dm(time(nullptr)) {}

	Buffer_t generate_input() {
		Buffer_t buf;
		unsigned with = _cli.digits_from + std::abs(_dm.lrand48() / 16) % (_cli.digits_to - _cli.digits_from + 1u);
		if(with > 0) {
			--with;
			buf.push_back((std::abs(_dm.lrand48()) % 9u) + 1u);
			while(with--) {
				buf.push_back(std::abs(_dm.lrand48()) % 10u);
			}
		}
		return buf;
	}

	String_t generate_question(const Buffer_t& buf) const {
		String_t question;

		switch(_cli.action.action().value) {
			case NihongoNoSujiCli::EnumMethod::DIGITS:
				switch(_cli.question.value().value) {
					case NihongoNoSujiCli::EnumQuestion::ARABIC:   write_digits(buf, DIGIT_MAP_ARABIC, question);     break;
					case NihongoNoSujiCli::EnumQuestion::HIRAGANA: write_digits(buf, DIGIT_MAP_HIRAGANA, question);   break;
					case NihongoNoSujiCli::EnumQuestion::KANJI:    write_digits(buf, DIGIT_MAP_KANJI, question);      break;
					case NihongoNoSujiCli::EnumQuestion::AUDIO:    write_digits(buf, DIGIT_MAP_ARABIC_SEP, question); break;
					default: assert(false); break;
				}
				break;

			case NihongoNoSujiCli::EnumMethod::NUMBERS:
				switch(_cli.question.value().value) {
					case NihongoNoSujiCli::EnumQuestion::ARABIC:   write_digits(buf, DIGIT_MAP_ARABIC, question); break;
					case NihongoNoSujiCli::EnumQuestion::HIRAGANA: write_number_hiragana(buf, question);          break;
					case NihongoNoSujiCli::EnumQuestion::KANJI:    write_number_kanji(buf, question);             break;
					case NihongoNoSujiCli::EnumQuestion::AUDIO:    write_digits(buf, DIGIT_MAP_ARABIC, question); break;
					default: assert(false); break;
				}
				break;

			default: assert(false); break;
		}

		return question;
	}

	String_t generate_reference(const Buffer_t& buf) const {
		String_t reference;

		switch(_cli.action.action().value) {
			case NihongoNoSujiCli::EnumMethod::DIGITS:
				switch(_cli.answer.value().value) {
					case NihongoNoSujiCli::EnumAnswer::ARABIC:   write_digits(buf, DIGIT_MAP_ARABIC, reference);   break;
					case NihongoNoSujiCli::EnumAnswer::HIRAGANA: write_digits(buf, DIGIT_MAP_HIRAGANA, reference); break;
					case NihongoNoSujiCli::EnumAnswer::KANJI:    write_number_kanji(buf, reference);               break;
					default: assert(false); break;
				}
				break;

			case NihongoNoSujiCli::EnumMethod::NUMBERS:
				switch(_cli.answer.value().value) {
					case NihongoNoSujiCli::EnumAnswer::ARABIC:   write_digits(buf, DIGIT_MAP_ARABIC, reference); break;
					case NihongoNoSujiCli::EnumAnswer::HIRAGANA: write_number_hiragana(buf, reference);          break;
					case NihongoNoSujiCli::EnumAnswer::KANJI:    write_number_kanji(buf, reference);             break;
					default: assert(false); break;
				}
				break;

			default: assert(false); break;
		}

		return reference;
	}

	String_t filter_kanji(const String_t& in) const {
		String_t result;
		for(const auto ch : in) {
			switch(ch) {
				case U'ニ':
					result.push_back(U'二');
					break;

				case U'ー':
					result.push_back(U'一');
					break;

				default:
					result.push_back(ch);
					break;
			}
		}
		return result;
	}

	void run() {
		auto tm_before = time(nullptr);

		const unsigned rounds_total = _cli.rounds;
		unsigned rounds_left = _cli.rounds;
		unsigned mistakes = 0;
		while(rounds_left--) {

			const Buffer_t input = generate_input();
			const String_t question = generate_question(input);
			const String_t reference = generate_reference(input);

			if(_cli.question.value().value == NihongoNoSujiCli::EnumQuestion::AUDIO) {
				say(question);
			} else {
				printf("%-*s ", int(_cli.digits_to), to_basic_string(question).c_str());
				fflush(stdout);
			}

			// Read the output.
			String_t output;
			read_line(stdin, output, true);
			output = filter_kanji(output);

			// Check the result.
			while(output != reference) {
				++mistakes;
				printf("%s", TermColor::front(TermColor::RED));
				printf("%s", to_basic_string(reference).c_str());
				printf("\n%s", TermColor::reset());

				if(_cli.question.value().value == NihongoNoSujiCli::EnumQuestion::AUDIO) {
					say(question);
				}
				read_line(stdin, output, true);
				output = filter_kanji(output);
			}
			printf("\n");
		}

		double miskates_percent = mistakes;
		miskates_percent /= rounds_total;
		miskates_percent *= 100;

		printf("Mistakes : %u of %u (%.2f%%).", mistakes, rounds_total, miskates_percent);
		const unsigned seconds_total = time(nullptr) - tm_before;
		printf(" %u seconds.\n", seconds_total);
	}

	// private:

	void generate_test_input(Buffer_t& buf) {
		static unsigned text_idx = 0;
		switch(text_idx % 9u) {
			case 0:
				buf = {1};
				break;

			case 1:
				buf = {1,1};
				// buf = {1, 0};
				break;

			case 2:
				buf = {1,1,1};
				// buf = {1, 0, 0};
				break;

			case 3:
				buf = {1,1,1,1};
				// buf = {1, 0, 0, 0};
				break;

			case 4:
				buf = {1,1,1,1,1};
				// buf = {1, 0, 0, 0, 0};
				break;

			case 5:
				buf = {1,1,1,1,1,1};
				// buf = {1, 0, 0, 0, 0, 0};
				break;

			case 6:
				buf = {1,1,1,1,1,1,1};
				// buf = {1, 0, 0, 0, 0, 0, 0};
				break;

			case 7:
				buf = {1,1,1,1,1,1,1,1};
				// buf = {1, 0, 0, 0, 0, 0, 0, 0};
				break;

			case 8:
				buf = {1,1,1,1,1,1,1,1,1};
				// buf = {1, 0, 0, 0, 0, 0, 0, 0, 0};
				break;

		}
		++text_idx;
	}

	bool read_line(FILE* input, String_t& result, const bool skip_spaces) {
		std::string buf;
		int ch;
		while((ch = getc(input)) != EOF) {
			if(ch == '\n') {
				break;
			}
			if(skip_spaces && isspace(ch)) {
				continue;
			}
			buf.push_back(ch);
		}
		result = to_u32_string(buf);
		return ch != EOF;
	}


	template <typename M>
	static void write_digits(const Buffer_t& input, const M& map, String_t& output) {
		for(const auto& item : input) {
			output.append(map[item]);
		}
	}

	static void write_number_kanji(const Buffer_t& buf, String_t& output) {
		output.resize(0);

		bool has_man = false;

		for(size_t idx = 0; idx < buf.size(); ++idx) {
			const size_t exp = buf.size() - idx - 1u;
			// printf("exp=%zu buf[idx]=%u\n", exp, buf[idx]);

			switch(exp) {
				case 0u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					break;

				case 1u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"十");
					}
					break;

				case 2u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"百");
					}
					break;

				case 3u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"千");
					}
					break;

				case 4u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0 || has_man) {
						output.append(U"万");
					}
					break;

				case 5u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append(U"十");
					}
					break;

				case 6u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append(U"百");
					}
					break;

				case 7u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append(U"千");
					}
					break;

				case 8u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"億");
					}
					break;

				default:
					break;
			}
		}

		if(output.empty()) {
			output = U"ゼロ";
		}
	}

	static void write_number_hiragana(const Buffer_t& buf, String_t& output) {
		output.resize(0);

		bool has_man = false;

		for(size_t idx = 0; idx < buf.size(); ++idx) {
			const size_t exp = buf.size() - idx - 1u;
			// printf("exp=%zu buf[idx]=%u\n", exp, buf[idx]);

			switch(exp) {
				case 0u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					break;

				case 1u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"じゅう");
					}
					break;

				case 2u:
					switch(buf[idx]) {
						case 1u:
							output.append(U"ひゃく");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append(U"ひゃく");
							break;

						case 3u:
							output.append(U"さんびゃく");
							break;

						case 6u:
							output.append(U"ろっぴゃく");
							break;

						case 8u:
							output.append(U"はっぴゃく");
							break;

					}
					break;

				case 3u:
					switch(buf[idx]) {
						case 1u:
							output.append(U"せん");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 6u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append(U"せん");
							break;

						case 3u:
							output.append(U"さんぜん");
							break;

						case 8u:
							output.append(U"はっせん");
							break;

					}
					break;

				case 4u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0 || has_man) {
						output.append(U"まん");
					}
					break;

				case 5u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append(U"じゅう");
					}
					break;

				case 6u:
					if(buf[idx] > 0) {
						has_man = true;
					}

					switch(buf[idx]) {
						case 1u:
							output.append(U"ひゃく");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append(U"ひゃく");
							break;

						case 3u:
							output.append(U"さんびゃく");
							break;

						case 6u:
							output.append(U"ろっぴゃく");
							break;

						case 8u:
							output.append(U"はっぴゃく");
							break;
					}
					break;

				case 7u:
					if(buf[idx] > 0) {
						has_man = true;
					}

					switch(buf[idx]) {
						case 1u:
							output.append(U"せん");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 6u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append(U"せん");
							break;

						case 3u:
							output.append(U"さんぜん");
							break;

						case 8u:
							output.append(U"はっせん");
							break;
					}

					break;

				case 8u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append(U"おく");
					}
					break;

				default:
					break;
			}
		}

		if(output.empty()) {
			output = U"ゼロ";
		}
	}

	void say(const String_t& to_say) const {
		std::string command("trans -b -p  :en :jpn \"");
		command.append(to_basic_string(to_say));
		command.append("\" >> /dev/null");

		const auto err = system(command.c_str());
		if(err != EXIT_SUCCESS) {
			fprintf(stderr, "system(\"%s\") fails\n", command.c_str());
			exit(err);
		}
	}

	static std::string to_basic_string(const std::u32string& str) {
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.to_bytes(str);
	}

	static std::u32string to_u32_string(const std::string& str) {
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(str);
	}

};

int main(int argc, char** argv) {
	NihongoNoSujiCli cli;

	if(not cli.parse_args(argc, argv)) {
		cli.print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	NihongoNoSuji app(cli);
	app.run();

	return EXIT_SUCCESS;
}

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
		unsigned width = _cli.digits_from + std::abs(_dm.lrand48() / 16) % (_cli.digits_to - _cli.digits_from + 1u);
		if(width > 0) {
			--width;
			buf.push_back((std::abs(_dm.lrand48()) % 9u) + 1u);
			while(width--) {
				buf.push_back(std::abs(_dm.lrand48()) % 10u);
			}
		}
		return buf;
	}

	void time_generate_input(unsigned& hours, unsigned& min) {
		hours = std::abs(_dm.lrand48() % 24);
		if(_dm.pass(0.1)) {
			min = 30u;
		} else {
			min = std::abs(_dm.lrand48() % 60);
		}
	}

	void show_before(const Buffer_t& buf) const {
		String_t question;

		switch(_cli.mode.value().get()) {

			case NihongoNoSujiCli::EnumMode::DIGITS:
				if(_cli.show_kanji_before.presented()) {
					write_digits(buf, DIGIT_MAP_KANJI, question);
				}

				if(_cli.show_kana_before.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_HIRAGANA, question);
				}

				if(_cli.show_arabic_before.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_ARABIC, question);
				}

				if(_cli.play_audio_before.presented()) {
					String_t to_say;
					write_digits(buf, DIGIT_MAP_ARABIC_SEP, to_say);
					say(to_say);
				}
				break;

			case NihongoNoSujiCli::EnumMode::NUMBERS:
				if(_cli.show_kanji_before.presented()) {
					write_number_kanji(buf, question);
				}

				if(_cli.show_kana_before.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_number_hiragana(buf, question);
				}

				if(_cli.show_arabic_before.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_ARABIC, question);
				}

				if(_cli.play_audio_before.presented()) {
					String_t to_say;
					write_digits(buf, DIGIT_MAP_ARABIC, to_say);
					say(to_say);
				}
				break;

			default:
				assert(false);
				break;
		}
		if(not question.empty()) {
			printf("%s  ", to_basic_string(question).c_str());
		}
	}

	void show_after(const Buffer_t& buf) const {
		String_t question;

		switch(_cli.mode.value().get()) {

			case NihongoNoSujiCli::EnumMode::DIGITS:

				if(_cli.show_kanji_after.presented()) {
					write_digits(buf, DIGIT_MAP_KANJI, question);
				}

				if(_cli.show_kana_after.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_HIRAGANA, question);
				}

				if(_cli.show_arabic_after.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_ARABIC, question);
				}

				if(not question.empty()) {
					printf("%s\n", to_basic_string(question).c_str());
					fflush(stdout);
				}

				if(_cli.play_audio_after.presented()) {
					String_t to_say;
					write_digits(buf, DIGIT_MAP_ARABIC_SEP, to_say);
					say(to_say);
				}

				break;

			case NihongoNoSujiCli::EnumMode::NUMBERS:
				if(_cli.show_kanji_after.presented()) {
					write_number_kanji(buf, question);
				}

				if(_cli.show_kana_after.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_number_hiragana(buf, question);
				}

				if(_cli.show_arabic_after.presented()) {
					if(not question.empty()) {
						question.append(U"  ");
					}
					write_digits(buf, DIGIT_MAP_ARABIC, question);
				}
				if(not question.empty()) {
					printf("%s\n", to_basic_string(question).c_str());
					fflush(stdout);
				}

				if(_cli.play_audio_after.presented()) {
					String_t to_say;
					write_digits(buf, DIGIT_MAP_ARABIC, to_say);
					say(to_say);
				}

				break;

			default:
				assert(false);
				break;
		}

	}

	void run() {
		auto tm_before = time(nullptr);

		const unsigned rounds_total = _cli.rounds;
		unsigned rounds_left = _cli.rounds;
		unsigned mistakes = 0;
		while(rounds_left--) {

			if(_cli.mode.value().get() == NihongoNoSujiCli::EnumMode::TIME) {
				unsigned hours_24 = 0;
				unsigned hours_12 = 0;
				unsigned min = 0;
				time_generate_input(hours_24, min);

				String_t to_say;
				if(hours_24 < 12u) {
					hours_12 = hours_24;
					to_say.append(U"午前");
				} else {
					hours_12 = hours_24 - 12u;
					to_say.append(U"午後");
				}
				to_say.append(to_u32_string(std::to_string(hours_12)));
				to_say.append(U"時");

				switch(min) {
					case 0:
						break;

					case 30:
						to_say.append(U"半");
						break;

					default:
						to_say.append(to_u32_string(std::to_string(min)));
						to_say.append(U"分");
						break;
				}

				String_t reference;
				if(hours_24 < 10) {
					reference.push_back('0');
				}
				reference.append(to_u32_string(std::to_string(hours_24)));
				reference.push_back(':');
				if(min < 10) {
					reference.push_back('0');
				}
				reference.append(to_u32_string(std::to_string(min)));

				if(_cli.show_arabic_before.presented()) {
					printf("%s ", to_basic_string(reference).c_str());
					fflush(stdout);
				}

				if(_cli.show_kanji_before.presented()) {
					printf("%s ", to_basic_string(to_say).c_str());
					fflush(stdout);
				}

				if(_cli.play_audio_before.presented()) {
					say(to_say);
				}

				// Read the output.
				String_t output;
				read_line(stdin, output, true);

				if(_cli.action.action().value == NihongoNoSujiCli::EnumMethod::TEST) {
					// Check the result.
					while (output != reference) {
						++mistakes;
						printf("%s", TermColor::front(TermColor::RED));
						printf("%s", to_basic_string(reference).c_str());
						printf("\n%s", TermColor::reset());

						if(_cli.show_arabic_before.presented()) {
							printf("%s ", to_basic_string(reference).c_str());
							fflush(stdout);
						}

						if(_cli.show_kanji_before.presented()) {
							printf("%s ", to_basic_string(to_say).c_str());
							fflush(stdout);
						}

						if(_cli.play_audio_before.presented()) {
							say(to_say);
						}

						fflush(stdout);
						read_line(stdin, output, true);
					}
					printf("\n");
				}

				if(_cli.show_arabic_after.presented()) {
					printf("%s ", to_basic_string(reference).c_str());
					fflush(stdout);
				}

				if(_cli.show_kanji_after.presented()) {
					printf("%s ", to_basic_string(to_say).c_str());
					fflush(stdout);
				}

				if(_cli.play_audio_after.presented()) {
					say(to_say);
				}

				continue;
			}

			const Buffer_t input = generate_input();
			String_t reference;
			write_digits(input, DIGIT_MAP_ARABIC, reference);

			show_before(input);
			fflush(stdout);

			// Read the output.
			String_t output;
			read_line(stdin, output, true);

			if(_cli.action.action().value == NihongoNoSujiCli::EnumMethod::TEST) {
				// Check the result.
				while (output != reference) {
					++mistakes;
					printf("%s", TermColor::front(TermColor::RED));
					printf("%s", to_basic_string(reference).c_str());
					printf("\n%s", TermColor::reset());

					show_before(input);
					fflush(stdout);
					read_line(stdin, output, true);
				}
				printf("\n");
			}

			show_after(input);

			if(_cli.wait_for_user.presented()) {
				fflush(stdout);
				printf("<ready>");
				read_line(stdin, output, true);
			}

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

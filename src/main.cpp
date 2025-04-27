#include "NihongoNoSujiCli.h"
#include "DiceMachine.h"
#include "TermColor.h"

#include <cstdio>
#include <vector>

class NihongoNoSuji {

	static constexpr const char* DIGIT_MAP_ARABIC_SEP[] = {"0 ", "1 ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 "};
	static constexpr const char* DIGIT_MAP_ARABIC[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	static constexpr const char* DIGIT_MAP_HIRAGANA[] = {"れい", "いち", "に", "さん", "よん", "ご", "ろく", "なな", "はち", "きゅう"};
	static constexpr const char* DIGIT_MAP_KANJI[] = {"0", "一", "二", "三", "四", "五", "六", "七", "八", "九"};

	using Buffer_t = std::vector<unsigned char>;

	const NihongoNoSujiCli _cli;
	DiceMachine _dm;

public:
	NihongoNoSuji(const NihongoNoSujiCli& cli) :
		_cli(cli), _dm(time(nullptr)) {}

	void run() {
		auto tm_before = time(nullptr);

		const unsigned rounds_total = _cli.rounds;
		unsigned rounds_left = _cli.rounds;
		unsigned correct = 0;
		while(rounds_left--) {

			// Show or play the input.
			Buffer_t input_buf;
			generate_input(input_buf);
			// generate_test_input(input_buf);
			show_input(input_buf);
			fflush(stdout);

			// Read the output.
			std::string output;
			read_output(output);

			// printf("(%s)\n", output.c_str());

			// Check the result.
			std::string reference;
			generate_reference(input_buf, reference);
			if(output == reference) {
				++correct;
			} else {
				printf("%s", TermColor::front(TermColor::RED));
				printf("%s", reference.c_str());
				printf("\n%s", TermColor::reset());
			}
			printf("\n");
		}

		double correct_percent = correct;
		correct_percent /= rounds_total;
		correct_percent *= 100;

		printf("Correct : %u of %u (%.2f%%).", correct, rounds_total, correct_percent);
		const unsigned seconds_total = time(nullptr) - tm_before;
		printf(" %u seconds.\n", seconds_total);
	}

	// private:

	void generate_input(Buffer_t& buf) {
		buf.resize(0);
		unsigned with = _cli.digits_from + std::abs(_dm.lrand48() / 16) % (_cli.digits_to - _cli.digits_from + 1u);
		if(with > 0) {
			--with;
			buf.push_back((std::abs(_dm.lrand48()) % 9u) + 1u);
			while(with--) {
				buf.push_back(std::abs(_dm.lrand48()) % 10u);
			}
		}
	}

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

	void show_input(const Buffer_t& buf) {
		std::string input_string;
		switch(_cli.input_type.value.value) {
			case NihongoNoSujiCli::EnumInputType::ARABIC:
				print_digits(buf, DIGIT_MAP_ARABIC, input_string);
				printf("%-*s ", int(_cli.digits_to), input_string.c_str());
				break;

			case NihongoNoSujiCli::EnumInputType::HIRAGANA:
				switch(_cli.action.method.value) {
					case NihongoNoSujiCli::EnumMethod::DIGITS:
						print_digits(buf, DIGIT_MAP_HIRAGANA, input_string);
						break;

					case NihongoNoSujiCli::EnumMethod::NUMBERS:
						print_number_hiragana(buf, input_string);
						break;

					default:
						break;
				}
				printf("%s ", input_string.c_str());
				break;

			case NihongoNoSujiCli::EnumInputType::KANJI:
				switch(_cli.action.method.value) {
					case NihongoNoSujiCli::EnumMethod::DIGITS:
						print_digits(buf, DIGIT_MAP_KANJI, input_string);
						break;

					case NihongoNoSujiCli::EnumMethod::NUMBERS:
						print_number_kanji(buf, input_string);
						break;

					default:
						break;
				}
				printf("%s ", input_string.c_str());
				break;

			case NihongoNoSujiCli::EnumInputType::AUDIO:
				switch(_cli.action.method.value) {
					case NihongoNoSujiCli::EnumMethod::DIGITS:
						print_digits(buf, DIGIT_MAP_ARABIC_SEP, input_string);
						break;

					case NihongoNoSujiCli::EnumMethod::NUMBERS:
						print_digits(buf, DIGIT_MAP_ARABIC, input_string);
						break;

					default:
						break;
				}
				say(input_string);
				break;

			default:
				break;
		}
	}

	void read_output(std::string& buf) {
		buf.resize(0);
		int ch;
		while((ch = getchar()) != '\n') {
			if(not isspace(ch)) {
				buf.push_back(ch);
			}
		}
	}

	void generate_reference(const Buffer_t& buf, std::string& ref) const {
		ref.resize(0);
		switch(_cli.output_type.value.value) {
			case NihongoNoSujiCli::EnumOutputType::ARABIC:
				print_digits(buf, DIGIT_MAP_ARABIC, ref);
				break;

			case NihongoNoSujiCli::EnumOutputType::HIRAGANA:
				switch(_cli.action.method.value) {
					case NihongoNoSujiCli::EnumMethod::DIGITS:
						print_digits(buf, DIGIT_MAP_HIRAGANA, ref);
						break;

					case NihongoNoSujiCli::EnumMethod::NUMBERS:
						print_number_hiragana(buf, ref);
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
	}

	template <typename M>
	static void print_digits(const Buffer_t& buf, const M& map, std::string& output) {
		for(const auto& item : buf) {
			output.append(map[item]);
		}
	}

	static void print_number_kanji(const Buffer_t& buf, std::string& output) {
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
						output.append("十");
					}
					break;

				case 2u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append("百");
					}
					break;

				case 3u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append("千");
					}
					break;

				case 4u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0 || has_man) {
						output.append("万");
					}
					break;

				case 5u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append("十");
					}
					break;

				case 6u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append("百");
					}
					break;

				case 7u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append("千");
					}
					break;

				case 8u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_KANJI[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append("億");
					}
					break;

				default:
					break;
			}
		}

		if(output.empty()) {
			output = "ゼロ";
		}
	}

	static void print_number_hiragana(const Buffer_t& buf, std::string& output) {
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
						output.append("じゅう");
					}
					break;

				case 2u:
					switch(buf[idx]) {
						case 1u:
							output.append("ひゃく");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append("ひゃく");
							break;

						case 3u:
							output.append("さんびゃく");
							break;

						case 6u:
							output.append("ろっぴゃく");
							break;

						case 8u:
							output.append("はっぴゃく");
							break;

					}
					break;

				case 3u:
					switch(buf[idx]) {
						case 1u:
							output.append("せん");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 6u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append("せん");
							break;

						case 3u:
							output.append("さんぜん");
							break;

						case 8u:
							output.append("はっせん");
							break;

					}
					break;

				case 4u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0 || has_man) {
						output.append("まん");
					}
					break;

				case 5u:
					if(buf[idx] > 1) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0) {
						has_man = true;
						output.append("じゅう");
					}
					break;

				case 6u:
					if(buf[idx] > 0) {
						has_man = true;
					}

					switch(buf[idx]) {
						case 1u:
							output.append("ひゃく");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append("ひゃく");
							break;

						case 3u:
							output.append("さんびゃく");
							break;

						case 6u:
							output.append("ろっぴゃく");
							break;

						case 8u:
							output.append("はっぴゃく");
							break;
					}
					break;

				case 7u:
					if(buf[idx] > 0) {
						has_man = true;
					}

					switch(buf[idx]) {
						case 1u:
							output.append("せん");
							break;

						case 2u:
						case 4u:
						case 5u:
						case 6u:
						case 7u:
						case 9u:
							output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
							output.append("せん");
							break;

						case 3u:
							output.append("さんぜん");
							break;

						case 8u:
							output.append("はっせん");
							break;
					}

					break;

				case 8u:
					if(buf[idx] > 0) {
						output.append(DIGIT_MAP_HIRAGANA[buf[idx]]);
					}
					if(buf[idx] > 0) {
						output.append("おく");
					}
					break;

				default:
					break;
			}
		}

		if(output.empty()) {
			output = "ゼロ";
		}
	}

	void say(const std::string& to_say) {
		std::string command("trans -b -p  :en :jpn \"");
		command.append(to_say);
		command.append("\" >> /dev/null");

		const auto err = system(command.c_str());
		if(err != EXIT_SUCCESS) {
			fprintf(stderr, "system(\"%s\") fails\n", command.c_str());
			exit(err);
		}
	}

};

int main(int argc, char** argv) {
	NihongoNoSujiCli cli;

	if(not cli.parse_args(argc, argv)) {
		cli.print_usage(stderr);
		return EXIT_FAILURE;
	}

	NihongoNoSuji app(cli);
	app.run();

	return EXIT_SUCCESS;
}

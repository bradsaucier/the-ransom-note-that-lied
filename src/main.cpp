#include "ransom_note/analyzer.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

int main(const int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ransom-note-checker <note-file> <claimed-source-file>\n";
    return 64;
  }

  try {
    const std::filesystem::path note_path(argv[1]);
    const std::filesystem::path source_path(argv[2]);
    const std::string note = ransom_note::read_ascii_text_file(note_path);
    const std::string source = ransom_note::read_ascii_text_file(source_path);
    const ransom_note::Analysis result = ransom_note::analyze_text(note, source);

    std::cout << "Note words examined: " << result.note_word_count << '\n';
    std::cout << "Source words examined: " << result.source_word_count << '\n';

    if (result.possible) {
      std::cout << "VERDICT: POSSIBLE\n";
      std::cout << "The claimed source contains enough copies of every note word.\n";
      return 0;
    }

    std::cout << "VERDICT: THE NOTE LIED\n";
    std::cout << "The claimed source cannot supply the following inventory:\n";
    for (std::size_t index = 0; index < result.shortages.size(); ++index) {
      const ransom_note::Shortage& shortage = result.shortages.at(index);
      std::cout << index + 1U << ". " << shortage.word
                << " required=" << shortage.required
                << " available=" << shortage.available << '\n';
    }
    return 2;
  } catch (const std::exception& error) {
    std::cerr << "Analysis failed: " << error.what() << '\n';
    return 1;
  }
}

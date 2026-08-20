#include "ransom_note/analyzer.hpp"

#include <array>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace ransom_note {
namespace {

bool is_ascii_alphanumeric(const unsigned char value) {
  return (value >= static_cast<unsigned char>('A') && value <= static_cast<unsigned char>('Z')) ||
         (value >= static_cast<unsigned char>('a') && value <= static_cast<unsigned char>('z')) ||
         (value >= static_cast<unsigned char>('0') && value <= static_cast<unsigned char>('9'));
}

char to_ascii_lower(const unsigned char value) {
  if (value >= static_cast<unsigned char>('A') && value <= static_cast<unsigned char>('Z')) {
    const auto offset = static_cast<unsigned char>(value - static_cast<unsigned char>('A'));
    return static_cast<char>(static_cast<unsigned char>('a') + offset);
  }
  return static_cast<char>(value);
}

void append_word(
    std::vector<std::string>& words,
    std::string& current,
    const std::size_t max_words) {
  if (current.empty()) {
    return;
  }
  if (words.size() >= max_words) {
    throw std::length_error("input contains more words than the configured limit");
  }
  words.push_back(std::move(current));
  current.clear();
}

}  // namespace

std::string read_ascii_text_file(
    const std::filesystem::path& path,
    const std::uintmax_t max_bytes) {
  if (max_bytes == 0U) {
    throw std::invalid_argument("maximum file size must be greater than zero");
  }

  std::error_code error;
  const bool regular_file = std::filesystem::is_regular_file(path, error);
  if (error || !regular_file) {
    throw std::runtime_error("input path is not a readable regular file: " + path.string());
  }

  const std::uintmax_t reported_size = std::filesystem::file_size(path, error);
  if (error) {
    throw std::runtime_error("unable to determine input file size: " + path.string());
  }
  if (reported_size > max_bytes) {
    throw std::length_error("input file exceeds the configured size limit: " + path.string());
  }
  if (reported_size > static_cast<std::uintmax_t>(std::numeric_limits<std::size_t>::max())) {
    throw std::length_error("input file is too large for this platform");
  }

  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error("unable to open input file: " + path.string());
  }

  std::string contents;
  contents.reserve(static_cast<std::size_t>(reported_size));
  std::array<char, 4096> buffer{};

  while (input) {
    input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    const std::streamsize bytes_read = input.gcount();
    if (bytes_read < 0) {
      throw std::runtime_error("input stream returned an invalid byte count");
    }

    const auto chunk_size = static_cast<std::uintmax_t>(bytes_read);
    const auto current_size = static_cast<std::uintmax_t>(contents.size());
    if (chunk_size > max_bytes || current_size > max_bytes - chunk_size) {
      throw std::length_error("input file changed and exceeded the configured size limit");
    }

    contents.append(buffer.data(), static_cast<std::size_t>(bytes_read));
  }

  if (!input.eof()) {
    throw std::runtime_error("unable to read the complete input file: " + path.string());
  }

  return contents;
}

std::vector<std::string> tokenize_ascii(
    const std::string_view text,
    const std::size_t max_words) {
  if (max_words == 0U) {
    throw std::invalid_argument("maximum word count must be greater than zero");
  }

  std::vector<std::string> words;
  std::string current;

  for (const char raw_value : text) {
    const auto value = static_cast<unsigned char>(raw_value);
    if (value > 127U) {
      throw std::invalid_argument("input must use ASCII text");
    }

    if (is_ascii_alphanumeric(value)) {
      current.push_back(to_ascii_lower(value));
    } else {
      append_word(words, current, max_words);
    }
  }

  append_word(words, current, max_words);
  return words;
}

Inventory build_inventory(const std::vector<std::string>& words) {
  Inventory inventory;
  for (const std::string& word : words) {
    std::size_t& count = inventory[word];
    if (count == std::numeric_limits<std::size_t>::max()) {
      throw std::overflow_error("word frequency exceeds the supported range");
    }
    ++count;
  }
  return inventory;
}

Analysis analyze_tokens(
    const std::vector<std::string>& note_words,
    const std::vector<std::string>& source_words) {
  if (note_words.empty()) {
    throw std::invalid_argument("the note contains no words");
  }
  if (source_words.empty()) {
    throw std::invalid_argument("the claimed source contains no words");
  }

  const Inventory note_inventory = build_inventory(note_words);
  const Inventory source_inventory = build_inventory(source_words);
  std::vector<Shortage> shortages;

  for (const auto& [word, required] : note_inventory) {
    const auto source_entry = source_inventory.find(word);
    const std::size_t available =
        source_entry == source_inventory.end() ? 0U : source_entry->second;
    if (available < required) {
      shortages.push_back(Shortage{word, required, available});
    }
  }

  return Analysis{
      shortages.empty(),
      note_words.size(),
      source_words.size(),
      std::move(shortages)};
}

Analysis analyze_text(const std::string_view note, const std::string_view source) {
  return analyze_tokens(tokenize_ascii(note), tokenize_ascii(source));
}

}  // namespace ransom_note

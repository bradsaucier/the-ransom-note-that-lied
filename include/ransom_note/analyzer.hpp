#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace ransom_note {

inline constexpr std::uintmax_t kDefaultMaxInputBytes = 1'048'576;
inline constexpr std::size_t kDefaultMaxWords = 100'000;

using Inventory = std::map<std::string, std::size_t>;

struct Shortage {
  std::string word;
  std::size_t required;
  std::size_t available;

  bool operator==(const Shortage&) const = default;
};

struct Analysis {
  bool possible;
  std::size_t note_word_count;
  std::size_t source_word_count;
  std::vector<Shortage> shortages;
};

std::string read_ascii_text_file(
    const std::filesystem::path& path,
    std::uintmax_t max_bytes = kDefaultMaxInputBytes);

std::vector<std::string> tokenize_ascii(
    std::string_view text,
    std::size_t max_words = kDefaultMaxWords);

Inventory build_inventory(const std::vector<std::string>& words);

Analysis analyze_tokens(
    const std::vector<std::string>& note_words,
    const std::vector<std::string>& source_words);

Analysis analyze_text(std::string_view note, std::string_view source);

}  // namespace ransom_note

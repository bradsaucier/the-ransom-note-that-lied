#include "ransom_note/analyzer.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::filesystem::path test_data(const std::string& filename) {
  return std::filesystem::path(RNTL_TEST_DATA_DIR) / filename;
}

TEST(Tokenization, EmptyTextProducesNoWords) {
  EXPECT_TRUE(ransom_note::tokenize_ascii("").empty());
}

TEST(Tokenization, NormalizesCaseAndPunctuation) {
  const std::vector<std::string> expected{"meet", "at", "midnight", "room", "7"};
  EXPECT_EQ(expected, ransom_note::tokenize_ascii("MEET, at midnight! Room 7."));
}

TEST(Tokenization, RejectsNonAsciiInput) {
  const std::string input{"evidence\xC3\xA9"};
  EXPECT_THROW((void)ransom_note::tokenize_ascii(input), std::invalid_argument);
}

TEST(Tokenization, EnforcesWordLimit) {
  EXPECT_THROW((void)ransom_note::tokenize_ascii("one two", 1U), std::length_error);
}

TEST(Inventory, CountsRepeatedWords) {
  const ransom_note::Inventory inventory =
      ransom_note::build_inventory({"blue", "blue", "folder"});
  EXPECT_EQ(2U, inventory.at("blue"));
  EXPECT_EQ(1U, inventory.at("folder"));
}

TEST(Analysis, ExactInventoryPasses) {
  const ransom_note::Analysis result = ransom_note::analyze_text(
      "meet at midnight", "At midnight, meet.");
  EXPECT_TRUE(result.possible);
  EXPECT_TRUE(result.shortages.empty());
}

TEST(Analysis, ExtraSourceWordsDoNotChangeVerdict) {
  const ransom_note::Analysis result = ransom_note::analyze_text(
      "blue folder", "The old blue folder waits by the clock.");
  EXPECT_TRUE(result.possible);
}

TEST(Analysis, RepeatedWordShortageIsReported) {
  const ransom_note::Analysis result = ransom_note::analyze_text(
      "meet meet tonight", "Meet tonight.");
  ASSERT_FALSE(result.possible);
  ASSERT_EQ(1U, result.shortages.size());
  EXPECT_EQ((ransom_note::Shortage{"meet", 2U, 1U}), result.shortages.at(0));
}

TEST(Analysis, MultipleShortagesAreReportedInSortedOrder) {
  const ransom_note::Analysis result = ransom_note::analyze_text(
      "zebra alpha", "beta");
  ASSERT_EQ(2U, result.shortages.size());
  EXPECT_EQ("alpha", result.shortages.at(0).word);
  EXPECT_EQ("zebra", result.shortages.at(1).word);
}

TEST(Analysis, EmptyNoteIsRejected) {
  EXPECT_THROW((void)ransom_note::analyze_text("", "source"), std::invalid_argument);
}

TEST(Analysis, EmptySourceIsRejected) {
  EXPECT_THROW((void)ransom_note::analyze_text("note", ""), std::invalid_argument);
}

TEST(FileInput, ReadsKnownAsciiFile) {
  const std::string contents =
      ransom_note::read_ascii_text_file(test_data("claimed_source.txt"));
  EXPECT_NE(std::string::npos, contents.find("midnight"));
}

TEST(FileInput, RejectsMissingFile) {
  EXPECT_THROW(
      (void)ransom_note::read_ascii_text_file(test_data("missing.txt")),
      std::runtime_error);
}

TEST(FileInput, RejectsFileAboveConfiguredLimit) {
  EXPECT_THROW(
      (void)ransom_note::read_ascii_text_file(test_data("claimed_source.txt"), 8U),
      std::length_error);
}

TEST(FileInput, PlausibleSamplePasses) {
  const std::string note =
      ransom_note::read_ascii_text_file(test_data("note_plausible.txt"));
  const std::string source =
      ransom_note::read_ascii_text_file(test_data("claimed_source.txt"));
  EXPECT_TRUE(ransom_note::analyze_text(note, source).possible);
}

TEST(FileInput, LyingSampleNamesTheMissingWord) {
  const std::string note =
      ransom_note::read_ascii_text_file(test_data("note_lied.txt"));
  const std::string source =
      ransom_note::read_ascii_text_file(test_data("claimed_source.txt"));
  const ransom_note::Analysis result = ransom_note::analyze_text(note, source);
  ASSERT_FALSE(result.possible);
  ASSERT_EQ(1U, result.shortages.size());
  EXPECT_EQ("dawn", result.shortages.at(0).word);
}

}  // namespace

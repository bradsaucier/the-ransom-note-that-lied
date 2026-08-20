#include <gtest/gtest.h>

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace {

class CollectionTest : public ::testing::Test {
 protected:
  std::vector<int> collection;

  void add_entries(const int count) {
    ASSERT_GT(count, 0);
    for (int value = 0; value < count; ++value) {
      collection.push_back(value);
    }
  }
};

TEST_F(CollectionTest, CollectionExists) {
  EXPECT_TRUE(collection.empty());
}

TEST_F(CollectionTest, IsEmptyOnCreate) {
  EXPECT_TRUE(collection.empty());
  EXPECT_EQ(0U, collection.size());
}

TEST_F(CollectionTest, CanAddToEmptyVector) {
  add_entries(1);
  EXPECT_FALSE(collection.empty());
  EXPECT_EQ(1U, collection.size());
}

TEST_F(CollectionTest, CanAddFiveValuesToVector) {
  add_entries(5);
  EXPECT_EQ(5U, collection.size());
}

TEST_F(CollectionTest, MaxSizeIsAtLeastSize) {
  add_entries(10);
  EXPECT_GE(collection.max_size(), collection.size());
}

TEST_F(CollectionTest, CapacityIsAtLeastSize) {
  add_entries(10);
  EXPECT_GE(collection.capacity(), collection.size());
}

TEST_F(CollectionTest, ResizeIncreasesCollection) {
  add_entries(5);
  collection.resize(10);
  EXPECT_EQ(10U, collection.size());
}

TEST_F(CollectionTest, ResizeDecreasesCollection) {
  add_entries(10);
  collection.resize(5);
  EXPECT_EQ(5U, collection.size());
}

TEST_F(CollectionTest, ResizeDecreasesCollectionToZero) {
  add_entries(5);
  collection.resize(0);
  EXPECT_TRUE(collection.empty());
}

TEST_F(CollectionTest, ClearErasesCollection) {
  add_entries(5);
  collection.clear();
  EXPECT_TRUE(collection.empty());
}

TEST_F(CollectionTest, EraseBeginEndErasesCollection) {
  add_entries(5);
  collection.erase(collection.begin(), collection.end());
  EXPECT_TRUE(collection.empty());
}

TEST_F(CollectionTest, ReserveIncreasesCapacityButDoesNotChangeSize) {
  add_entries(5);
  const std::size_t original_size = collection.size();
  const std::size_t requested_capacity = collection.capacity() + 10U;
  collection.reserve(requested_capacity);
  EXPECT_EQ(original_size, collection.size());
  EXPECT_GE(collection.capacity(), requested_capacity);
}

TEST_F(CollectionTest, AtThrowsWhenIndexEqualsSize) {
  add_entries(1);
  EXPECT_THROW((void)collection.at(collection.size()), std::out_of_range);
}

TEST_F(CollectionTest, FrontAndBackReturnFirstAndLastValues) {
  collection.push_back(10);
  collection.push_back(20);
  collection.push_back(30);
  EXPECT_EQ(10, collection.front());
  EXPECT_EQ(30, collection.back());
}

TEST_F(CollectionTest, AtThrowsWhenCollectionIsEmpty) {
  EXPECT_THROW((void)collection.at(0), std::out_of_range);
}

}  // namespace

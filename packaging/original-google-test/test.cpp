// Uncomment the next line to use precompiled headers.
#include "pch.h"
// Uncomment the next line if you do not use precompiled headers.
//#include "gtest/gtest.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <vector>

// The global test environment setup and tear down.
// You should not need to change anything here.
class Environment : public ::testing::Environment
{
public:
  ~Environment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override
  {
    // Starter environment setup left intact.
    // The tests below never assert on a specific random value.
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {}
};

// Create our test class to house shared data between tests.
// You should not need to change anything here.
class CollectionTest : public ::testing::Test
{
protected:
  // Create a smart pointer to hold our collection.
  std::unique_ptr<std::vector<int>> collection;

  void SetUp() override
  {
    // Create a new collection to be used in the test.
    collection.reset(new std::vector<int>);
  }

  void TearDown() override
  {
    // Erase all elements in the collection, if any remain.
    collection->clear();

    // Free the pointer.
    collection.reset(nullptr);
  }

  // Helper function to add random values from 0 to 99 count times.
  void add_entries(int count)
  {
    assert(count > 0);
    for (auto i = 0; i < count; ++i)
    {
      collection->push_back(std::rand() % 100);
    }
  }
};

// When should you use the EXPECT_xxx or ASSERT_xxx macros?
// Use ASSERT when failure should terminate processing, such as the reason for the test case.
// Use EXPECT when failure should notify, but processing should continue.

// Starter baseline test: verify the fixture creates the vector smart pointer.
TEST_F(CollectionTest, CollectionSmartPointerIsNotNull)
{
  ASSERT_TRUE(collection);
  ASSERT_NE(collection.get(), nullptr);
}

// Starter baseline test: verify a collection is empty when created.
TEST_F(CollectionTest, IsEmptyOnCreate)
{
  ASSERT_TRUE(collection->empty());
  ASSERT_EQ(static_cast<std::size_t>(0), collection->size());
}

/*
 * This starter test exists only to demonstrate a failing test in Test Explorer.
 * It is commented out for submission so the completed suite can pass.
 *
 * TEST_F(CollectionTest, AlwaysFail)
 * {
 *   FAIL();
 * }
 */

// Assignment test 1 of 13 (template TODO): verify adding one value creates a one-item vector.
TEST_F(CollectionTest, CanAddToEmptyVector)
{
  ASSERT_TRUE(collection->empty());
  ASSERT_EQ(static_cast<std::size_t>(0), collection->size());

  add_entries(1);

  EXPECT_FALSE(collection->empty());
  EXPECT_EQ(static_cast<std::size_t>(1), collection->size());
}

// Assignment test 2 of 13 (template TODO): verify adding five values creates a five-item vector.
TEST_F(CollectionTest, CanAddFiveValuesToVector)
{
  add_entries(5);

  EXPECT_FALSE(collection->empty());
  EXPECT_EQ(static_cast<std::size_t>(5), collection->size());
}

// Assignment test 3 of 13 (template TODO): verify max_size is at least size.
TEST_F(CollectionTest, MaxSizeGreaterOrEqualToSizeForCommonEntryCounts)
{
  const int entryCounts[] = { 0, 1, 5, 10 };

  for (const int entryCount : entryCounts)
  {
    collection->clear();

    if (entryCount > 0)
    {
      add_entries(entryCount);
    }

    EXPECT_GE(collection->max_size(), collection->size())
      << "max_size fell below size at entryCount = " << entryCount;
  }
}

// Assignment test 4 of 13 (template TODO): verify capacity is at least size.
TEST_F(CollectionTest, CapacityGreaterOrEqualToSizeForCommonEntryCounts)
{
  const int entryCounts[] = { 0, 1, 5, 10 };

  for (const int entryCount : entryCounts)
  {
    collection->clear();

    if (entryCount > 0)
    {
      add_entries(entryCount);
    }

    EXPECT_GE(collection->capacity(), collection->size())
      << "capacity fell below size at entryCount = " << entryCount;
  }
}

// Assignment test 5 of 13 (template TODO): verify resize can increase the vector size.
TEST_F(CollectionTest, ResizeIncreasesCollection)
{
  add_entries(5);
  ASSERT_EQ(static_cast<std::size_t>(5), collection->size());

  collection->resize(10);

  EXPECT_EQ(static_cast<std::size_t>(10), collection->size());
}

// Assignment test 6 of 13 (template TODO): verify resize can decrease the vector size.
TEST_F(CollectionTest, ResizeDecreasesCollection)
{
  add_entries(10);
  ASSERT_EQ(static_cast<std::size_t>(10), collection->size());

  collection->resize(5);

  EXPECT_EQ(static_cast<std::size_t>(5), collection->size());
}

// Assignment test 7 of 13 (template TODO): verify resize can reduce the vector to zero.
TEST_F(CollectionTest, ResizeDecreasesCollectionToZero)
{
  add_entries(5);

  collection->resize(0);

  EXPECT_TRUE(collection->empty());
  EXPECT_EQ(static_cast<std::size_t>(0), collection->size());
}

// Assignment test 8 of 13 (template TODO): verify clear erases every element.
TEST_F(CollectionTest, ClearErasesCollection)
{
  add_entries(5);
  ASSERT_FALSE(collection->empty());

  collection->clear();

  EXPECT_TRUE(collection->empty());
  EXPECT_EQ(static_cast<std::size_t>(0), collection->size());
}

// Assignment test 9 of 13 (template TODO): verify erase(begin, end) erases every element.
TEST_F(CollectionTest, EraseBeginEndErasesCollection)
{
  add_entries(5);
  ASSERT_FALSE(collection->empty());

  collection->erase(collection->begin(), collection->end());

  EXPECT_TRUE(collection->empty());
  EXPECT_EQ(static_cast<std::size_t>(0), collection->size());
}

// Assignment test 10 of 13 (template TODO): verify reserve increases capacity only.
TEST_F(CollectionTest, ReserveIncreasesCapacityButDoesNotChangeSize)
{
  add_entries(5);

  const std::size_t originalSize = collection->size();
  const std::size_t requestedCapacity = collection->capacity() + 10;

  collection->reserve(requestedCapacity);

  EXPECT_EQ(originalSize, collection->size());
  EXPECT_GE(collection->capacity(), requestedCapacity);
}

// Assignment test 11 of 13 (template TODO), negative: verify at throws outside the range.
TEST_F(CollectionTest, AtThrowsOutOfRangeWhenIndexEqualsSize)
{
  add_entries(1);

  EXPECT_THROW((void)collection->at(collection->size()), std::out_of_range);
}

// Assignment test 12 of 13, custom positive: verify front and back report the
// first and last elements. This test pushes fixed values instead of calling
// add_entries, because front and back cannot be checked against random data.
TEST_F(CollectionTest, FrontAndBackReturnFirstAndLastValues)
{
  collection->push_back(10);
  collection->push_back(20);
  collection->push_back(30);

  ASSERT_EQ(static_cast<std::size_t>(3), collection->size());
  EXPECT_EQ(10, collection->front());
  EXPECT_EQ(30, collection->back());
}

// Assignment test 13 of 13, custom negative: verify empty access throws.
TEST_F(CollectionTest, AtThrowsOutOfRangeWhenCollectionIsEmpty)
{
  ASSERT_TRUE(collection->empty());

  EXPECT_THROW((void)collection->at(0), std::out_of_range);
}

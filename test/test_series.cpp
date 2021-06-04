#include "gtest/gtest.h"

#include "series.hpp"


class SeriesTest : public :: testing::Test
{
protected:
    virtual void SetUp() override
    {
        for (int idx = 0; idx < 100; idx++)
        {
            data.addData(idx * 10, 15 + idx * 100);
        }
    }

    virtual void TearDown() override
    {
        data.clearData();
    }

    TimestampedData data;
};


TEST_F(SeriesTest, TestSize)
{
    // Test for series size() function

    ASSERT_EQ(data.size(), 100);

    data.clearData();

    ASSERT_EQ(data.size(), 0);

    for (int idx = 0; idx < 10; idx++)
    {
        data.addData(idx, idx * 10);
        ASSERT_EQ(data.size(), idx + 1);
        ASSERT_EQ(data.getNewestTimestamp(), idx);
    }
};


TEST_F(SeriesTest, TestEndpoints)
{
    // Tests for the front / back of the data

    ASSERT_EQ(data.getOldestTimestamp(), 0);
    ASSERT_EQ(data.getNewestTimestamp(), 990);

    ASSERT_EQ(data.getOldestValue(), 15);
    ASSERT_EQ(data.getNewestValue(), 9915);
}


TEST_F(SeriesTest, TestMinMax)
{
    // Tests for minimum / maximum functionality

    // TODO
}


TEST_F(SeriesTest, TestIndexing)
{
    // Tests for binary search indexing functionality

    // TODO
}

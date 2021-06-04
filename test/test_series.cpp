#include <stdlib.h>
#include <stdio.h>

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

    bool isInOrder()
    {
        if (data.size() == 0) return true;

        int64_t ts = data.getOldestTimestamp();

        for (int idx = 0; idx < data.size(); idx++)
        {
            auto t = data.getTimestamp(idx);

            if (t < ts) return false;

            ts = t;
        }

        return true;
    }
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


TEST_F(SeriesTest, TestAccess)
{
    // Test accessor functions

    ASSERT_EQ(data.getOldestTimestamp(), 0);
    ASSERT_EQ(data.getNewestTimestamp(), 990);

    ASSERT_EQ(data.getOldestValue(), 15);
    ASSERT_EQ(data.getNewestValue(), 9915);

    for (size_t idx = 0; idx < data.size(); idx++)
    {
        ASSERT_EQ(data.getTimestamp(idx), idx * 10);
        ASSERT_FLOAT_EQ(data.getValue(idx), 15.0f + idx * 100);
    }

    // Check out of bounds access
    try
    {
        auto f = data.getValue(1000);
        ASSERT_EQ(f, 1.2345);
    }
    catch (std::out_of_range const &exception)
    {
        
    }
}


TEST_F(SeriesTest, TestMinMax)
{
    // Tests for minimum / maximum functionality

    // TODO
}


TEST_F(SeriesTest, TestIndexing)
{
    for (int ts = 0; ts < 990; ts++)
    {
        auto idx = data.getIndexForTimestamp(ts, TimestampedData::SEARCH_LEFT_TO_RIGHT);

        ASSERT_EQ(idx, (int) ((ts + 10) / 10));
    }

    ASSERT_TRUE(isInOrder());

    data.clearData();
    ASSERT_TRUE(isInOrder());

    // Add linear samples
    for (int ts = 0; ts < 100; ts++)
    {
        data.addData(ts, rand() % 100);
    }

    ASSERT_TRUE(isInOrder());
    
    data.clearData();

    // Add random samples (must be inserted in order!)
    for (int ii = 0; ii < 10000; ii++)
    {
        data.addData(rand() % 1000, rand() % 1000);
    }

    ASSERT_TRUE(isInOrder());
}

//
// Created by Елизавета Кравченкова on 19.05.2024.
//

#include "lib/ComputerClubInfo.h"
#include <gtest/gtest.h>
#include <sstream>



//"../../test.txt"
TEST(ComputerClubTestSuite, SimpleErrorTest) {
    ComputerClubInfo info;
    info.Parse("testError.txt");
    ASSERT_THROW(info.Analysis(),parce_error);
}


TEST(ComputerClubTestSuite, SimpleErrorCheckStrTest) {
    ComputerClubInfo info;
    try{
        info.Parse("testError.txt");
        info.Analysis();
    }

    catch (parce_error ex) {
        std::string ans="09:54 2 client1 10";
        ASSERT_EQ(ex.what(),ans);
    }
}

TEST(ComputerClubTestSuite, SimpleRunTest) {
    ComputerClubInfo info;
    info.Parse("test.txt");
    info.Analysis();
    ASSERT_EQ(info.getTableCount(),3);
    ASSERT_EQ(info.getOneHourCost(),10);

    time_t start_time = info.getStartTime();
    struct tm* startTime = localtime(&start_time);
    ASSERT_EQ(startTime->tm_hour,9);
    ASSERT_EQ(startTime->tm_min,0);

    time_t end_time = info.getEndTime();
    struct tm* endTime = localtime(&end_time);
    ASSERT_EQ(startTime->tm_hour,19);
    ASSERT_EQ(startTime->tm_min,0);

    std::vector<ComputerClubEvent> events = info.getEvents();
    std::vector<uint64_t> ans_for_idEvents = {1,13,1,1,3,13,2,2,1,2,1,2,13,3,4,12,4,4,11};

    ASSERT_EQ(info.getEvents().size(),19);
    for (uint64_t i=0;i<events.size();++i){
        ASSERT_EQ(events[i].id,ans_for_idEvents[i]);
    }

    std::vector<Table> tables= info.getTables();
    ASSERT_EQ(tables[1].profit,70);
    ASSERT_EQ(tables[2].profit,30);
    ASSERT_EQ(tables[3].profit,90);
}

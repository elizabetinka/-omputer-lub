//
// Created by Елизавета Кравченкова on 19.05.2024.
//

#pragma once
#include <fstream>
#include <ctime>
#include <iostream>
#include <map>
#include <unordered_map>

enum InClub{
    NotHaveTable=-2,
    Wait=-1,
};

struct ComputerClubEvent{
    uint64_t id;
    std::time_t start_time;
    std::string body;
public:
    ComputerClubEvent(uint64_t id, time_t startTime, const std::string& body);
};

struct Table{
    uint64_t profit=0;
    std::time_t using_time= time(NULL);
    Table();
};


class ComputerClubInfo {
private:
    uint64_t tableCount=0;
    uint64_t oneHourCost=0;
    std::time_t start_time= time(NULL);
    std::time_t end_time=time(NULL);
    std::vector<ComputerClubEvent>  events;
    std::vector<Table>  tables;

    void AnalysisEvent(std::map<std::string,int64_t>& people_in_club,std::vector<ComputerClubEvent>& events_res,const ComputerClubEvent& event,std::unordered_map<uint64_t ,time_t>& start_in_table,uint64_t& table_use,uint64_t& in_queqe);
public:
    void Parse(const char* filename);

    void Analysis();

    void Print();
};


class parce_error: public std::exception
{
public:
    parce_error(const std::string& message): message{message}
    {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;
};
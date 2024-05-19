//
// Created by Елизавета Кравченкова on 19.05.2024.
//

#include <sstream>
#include "ComputerClubInfo.h"


time_t ComputerClubInfo::getTimeSep(std::ifstream& fin, char sep) {
    std::string time_str;
    std::getline(fin, time_str, sep);
    time_t tmp = time(NULL);
    std::tm* t = std::localtime(&tmp);
    std::istringstream ss(time_str);
    ss >> std::get_time(t, "%H:%M");
    t->tm_gmtoff = 0;
    t->tm_mday = 2;
    return mktime(t);
}

std::string ComputerClubInfo::getEventString(const ComputerClubEvent& event) {
    struct tm* t = localtime(&event.start_time);
    std::ostringstream ss;
    ss << std::put_time(t, "%H:%M");
    std::string ans = ss.str();
    ans += ' ' + std::to_string(event.id) + ' ' + event.body;
    return ans;
}

void ComputerClubInfo::Parse(const char* filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        throw std::runtime_error("can't open file");
    }
    size_t pos = fin.tellg();
    try {
        fin >> tableCount;
        pos = fin.tellg();
        start_time = getTimeSep(fin, ' ');
        end_time = getTimeSep(fin, '\n');
        pos = fin.tellg();
        fin >> oneHourCost;

        while (!fin.eof()) {
            pos = fin.tellg();
            time_t tmp = getTimeSep(fin, ' ');
            std::tm* t = localtime(&tmp);
            uint64_t id;
            fin >> id;
            if (id == 0 || id > 5) {
                throw std::runtime_error("id of coming events can be only 1,2,3,4");
            }

            std::string body;
            std::getline(fin, body, '\n');
            body.erase(0, 1);
            events.emplace_back(id, tmp, body);
        }
    }
    catch (...) {
        fin.seekg(pos, std::ios_base::beg);
        std::string ex;
        std::getline(fin, ex);
        fin.close();
        throw parce_error(ex);
    }
    fin.close();
}

void ComputerClubInfo::Analysis() {
    uint64_t table_use = 0;
    uint64_t in_queqe = 0;
    std::map<std::string, int64_t> people_in_club;
    std::vector<ComputerClubEvent> events_res;
    tables.assign(tableCount + 1, Table());
    std::unordered_map<uint64_t, time_t> start_in_table;
    for (uint32_t i = 0; i < events.size(); ++i) {
        AnalysisEvent(people_in_club, events_res, events[i], start_in_table, table_use, in_queqe);
    }
    for (const auto& [key, value]: people_in_club) {
        events_res.emplace_back(11, end_time, key);
        if (value > 0) {
            double diff = std::difftime(end_time, start_in_table[value]);
            uint64_t hours = diff / 3600;
            uint64_t minuts = static_cast<uint64_t>(diff) % 3600;
            if (minuts != 0) {
                tables[value].profit += oneHourCost;
            }
            tables[value].profit += hours * oneHourCost;
            tables[value].using_time += diff;
        }

    }

    events = events_res;
    people_in_club.clear();
    start_in_table.clear();
}

void ComputerClubInfo::AnalysisEvent(std::map<std::string, int64_t>& people_in_club,
                                     std::vector<ComputerClubEvent>& events_res, const ComputerClubEvent& event,
                                     std::unordered_map<uint64_t, time_t>& start_in_table, uint64_t& table_use,
                                     uint64_t& in_queqe) {
    events_res.push_back(event);


    switch (event.id) {
        case 1:
            if (people_in_club.contains(event.body)) {
                events_res.emplace_back(13, event.start_time, "YouShallNotPass");
            } else if (event.start_time < start_time || event.start_time > end_time) {
                events_res.emplace_back(13, event.start_time, "NotOpenYet");
            } else {
                people_in_club.insert(std::pair<std::string, int64_t>(event.body, NotHaveTable));
            }
            break;
        case 2: {
            std::string client;
            std::stringstream ss(event.body);
            uint64_t table;

            ss >> client;
            ss >> table;

            if (table <= 0 || table > tableCount) {
                throw parce_error(getEventString(event));
            }
            if (!people_in_club.contains(client)) {
                events_res.emplace_back(13, event.start_time, "ClientUnknown");
                break;
            }
            bool find = false;
            for (const auto& [key, value]: people_in_club) {
                if (value == table) {
                    events_res.emplace_back(13, event.start_time, "PlaceIsBusy");
                    find = true;
                    break;
                }
            }
            if (find) {
                break;
            }
            if (people_in_club[client] < 0) {
                ++table_use;
            }
            if (people_in_club[client] != table) {
                start_in_table[table] = event.start_time;
            }
            people_in_club[client] = table;
            break;
        }
        case 3: {
            if (table_use < tableCount) {
                events_res.emplace_back(13, event.start_time, "ICanWaitNoLonger!");
                break;
            }
            if (in_queqe > tableCount) {
                events_res.emplace_back(11, event.start_time, event.body);
                break;
            }
            people_in_club[event.body] = Wait;
            ++in_queqe;
            break;
        }
        case 4: {
            if (!people_in_club.contains(event.body)) {
                events_res.emplace_back(13, event.start_time, "ClientUnknown");
                break;
            }
            --table_use;
            uint64_t free_table = people_in_club[event.body];

            double diff = std::difftime(event.start_time, start_in_table[free_table]);
            uint64_t hours = diff / 3600;
            uint64_t minuts = static_cast<uint64_t>(diff) % 3600;
            if (minuts != 0) {
                tables[free_table].profit += oneHourCost;
            }
            tables[free_table].profit += hours * oneHourCost;
            tables[free_table].using_time += diff;

            start_in_table.erase(free_table);
            people_in_club.erase(event.body);
            for (const auto& [key, value]: people_in_club) {
                if (value == Wait) {
                    people_in_club[key] = free_table;
                    start_in_table[free_table] = event.start_time;
                    ++table_use;
                    events_res.emplace_back(12, event.start_time, key + " " + std::to_string(free_table));
                    break;
                }
            }
            break;
        }
    }
}


void ComputerClubInfo::PrintTime(const std::time_t& time) {
    struct tm* t = localtime(&time);
    std::cout << std::put_time(t, "%H:%M");
}

void ComputerClubInfo::Print() {
    PrintTime(start_time);
    std::cout << '\n';
    for (uint64_t i = 0; i < events.size(); ++i) {
        PrintTime(events[i].start_time);
        std::cout << ' ' << events[i].id << ' ' << events[i].body << '\n';
    }
    PrintTime(end_time);
    std::cout << '\n';
    for (uint64_t i = 1; i <= tableCount; ++i) {
        std::cout << i << ' ' << tables[i].profit << ' ';
        PrintTime(tables[i].using_time);
        if (i != (tableCount)) {
            std::cout << '\n';
        }
    }
}

uint64_t ComputerClubInfo::getTableCount() const {
    return tableCount;
}

uint64_t ComputerClubInfo::getOneHourCost() const {
    return oneHourCost;
}

time_t ComputerClubInfo::getStartTime() const {
    return start_time;
}

time_t ComputerClubInfo::getEndTime() const {
    return end_time;
}

const std::vector<ComputerClubEvent>& ComputerClubInfo::getEvents() const {
    return events;
}

const std::vector<Table>& ComputerClubInfo::getTables() const {
    return tables;
}


ComputerClubEvent::ComputerClubEvent(uint64_t id, time_t startTime, const std::string& body) : id(id),
                                                                                               start_time(startTime),
                                                                                               body(body) {}

Table::Table() {
    time_t tmp = time(NULL);
    std::tm* t = localtime(&tmp);
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
    t->tm_gmtoff = 0;
    t->tm_mday = 2;
    using_time = mktime(t);
}

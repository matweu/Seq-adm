#pragma once
#include <iostream>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;

class TrainingNNinfo {
public:
    TrainingNNinfo()
    {
        _timecol = 0;
    }
    TrainingNNinfo(const std::string& data, unsigned int TimeCol, unsigned int elapstime)
    {
        this->_data = data;
        this->_timecol = TimeCol;
        this->_elapstime = elapstime;
    }
    void toJson(json& json_) const;
    void fromJson(const json& json_);
    std::string GetData();
    unsigned int GetTimeCollected();
    unsigned int GetelapsTime();
    void setData(std::string _data_value);
    void setTimeCollected(unsigned int _timecol_value);
    void setelapsTime(unsigned int _elapstime_value);

    void PrintInfo();

private:
    std::string _data;
    unsigned int _timecol;
    unsigned int _elapstime;


};
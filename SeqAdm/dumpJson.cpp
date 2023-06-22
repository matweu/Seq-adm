#include "dumpJson.h"

//сериализации
void TrainingNNinfo::toJson(json& json_) const {
    json_["DataTraining"] = _data;
    json_["TimeCollected"] = _timecol;
    json_["SetTimeToCollect"] = _elapstime;
}
// десериализации
void TrainingNNinfo::fromJson(const json& json_) {
    json_.at("DataTraining").get_to(_data);
    json_.at("TimeCollected").get_to(_timecol);
    json_.at("SetTimeToCollect").get_to(_elapstime);
}
std::string TrainingNNinfo::GetData()
{
    return _data;
}
unsigned int TrainingNNinfo::GetTimeCollected()
{
    return _timecol;
}
unsigned int TrainingNNinfo::GetelapsTime()
{
    return _elapstime;
}
void TrainingNNinfo::setData(std::string _data_value)
{
    this->_data = _data_value;
    
}
void TrainingNNinfo::setTimeCollected(unsigned int _timecol_value)
{
    this->_timecol = _timecol_value;

}
void TrainingNNinfo::setelapsTime(unsigned int _elapstime_value)
{
    this->_elapstime = _elapstime_value;

}
void TrainingNNinfo::PrintInfo()
{
    /* // ошибка
    printf_s("Дата начала сбора данных:%lu ", _data);
    printf_s("Выбранный промежуток времни сбора(UNIX data):%lu ", _timecol);
    printf_s("Процент выполненного сбора:%.2f", ((double)(_elapstime / _timecol)) * 100,"%");
    */
    std::cout << "Дата начала сбора данных: " << _data << std::endl;
    std::cout << "Выбранный промежуток времни сбора(UNIX data)/data :" << _elapstime << "|" << (double)_elapstime / 86400<<" day" << std::endl;
    std::cout << "Процент выполненного сбора: " << ((double)_timecol / _elapstime)*100<<"%" << std::endl;
}

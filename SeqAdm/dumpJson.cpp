#include "dumpJson.h"

//������������
void TrainingNNinfo::toJson(json& json_) const {
    json_["DataTraining"] = _data;
    json_["TimeCollected"] = _timecol;
    json_["SetTimeToCollect"] = _elapstime;
}
// ��������������
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
    /* // ������
    printf_s("���� ������ ����� ������:%lu ", _data);
    printf_s("��������� ���������� ������ �����(UNIX data):%lu ", _timecol);
    printf_s("������� ������������ �����:%.2f", ((double)(_elapstime / _timecol)) * 100,"%");
    */
    std::cout << "���� ������ ����� ������: " << _data << std::endl;
    std::cout << "��������� ���������� ������ �����(UNIX data)/data :" << _elapstime << "|" << (double)_elapstime / 86400<<" day" << std::endl;
    std::cout << "������� ������������ �����: " << ((double)_timecol / _elapstime)*100<<"%" << std::endl;
}

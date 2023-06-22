#include <iostream>
#include <execution>
#include <ctime>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <string.h>
#include <thread>
#include <condition_variable>
#include "dumpJson.h"
#include "SeqAdmfunc.h"
#include "CollectorEvent.h"

std::mutex mtx;
std::condition_variable cv;
bool dataWritten = false;
bool ret() { return dataWritten; }


std::mutex mtx2;
std::condition_variable cv2;
bool dataWritten2 = false;
bool ret2() { return dataWritten2; }

class Decision
{
public:
    //Decision() {}
   // virtual ~Decision() {}
    void virtual NeedsTrains(std::string infostring) = 0;

    int virtual getdecisionAnswer() = 0;

};

class Human
{
    void Pred(Decision& des)
    {
        des.getdecisionAnswer();
    }

};

class Decision_NN : public Decision
{
public:

    Decision_NN()
    {
        _elapstimeDecision = 0;
        _collectedtime = 0;
    }

    // ~Decision_NN() override  {}
    void NeedsTrains(std::string infostring) override
    {
        auto valueAnswer = NeedsTrainingModel(infostring);
        if (valueAnswer == YES)
        {
            _decisionNNAnswer = YES;
        }
        else if (valueAnswer == NO)
        {
            _decisionNNAnswer = NO;
        }
    }
    int virtual getdecisionAnswer() override
    {
        return _decisionNNAnswer;
    }
    std::string get_dataDecision()
    {
        return _dataDecision;
    }

    unsigned int get_elapstimeDecision()
    {
        return _elapstimeDecision;
    }
    unsigned int get_collectedtime()
    {
        return _collectedtime;
    }

    void set_dataDecision(std::string _dataDecision_value)
    {
        this->_dataDecision = _dataDecision_value;;
    }

    void set_collectedtime(unsigned int _collectedtime_value)
    {
        this->_collectedtime = _collectedtime_value;
    }

    void set_elapstimeDecision(unsigned int _elapstimeDecision_value)
    {
        this->_elapstimeDecision = _elapstimeDecision_value;
    }

    void printdes()
    {
        std::cout << _dataDecision << std::endl;
        std::cout << _collectedtime << std::endl;
        std::cout << _elapstimeDecision << std::endl;
    }

private:
    int _decisionNNAnswer;
    std::string _dataDecision;
    unsigned int _elapstimeDecision;
    unsigned int _collectedtime;
};


void OpenSourceModel(std::function<void(const std::string&)> MlPython, std::string mlfolder)
{
    try
    {
        std::string modelfolder = "model.py";
        std::string Dir = GeTNameDir();
        std::vector<char> Dirstr(Dir.c_str(), Dir.c_str() + Dir.size() + 1);
        const char* ch = Dirstr.data();

        std::ifstream open_py_model(modelfolder);
        if (open_py_model.is_open())
        {
            printf_s("Идет выгрузка данных!\n");
            MlPython(mlfolder);
        }
        else
        {
            printf_s("Файл model.py не найден по расположению:%s\n", ch); //Фикс
            system("pause");
            exit(1);
        }
    }
    catch (std::exception* fail_open_model)
    {
        std::cout << fail_open_model->what() << std::endl;
    }
}

void LoadPyMLModel(std::string ModelFolder)
{
    //Загрузка модели
    dataWritten = false;

    printf_s("Сработал LoadPyMLModel\n");
    std::string train_predict = "train.predict";
    std::string train_error = "train.error";
    std::string test_fname = "testevent.txt";
    std::string location = GeTNameDir();

    std::string command = "python -u " + location + char(92) + "model.py" + " " + ModelFolder + " " + test_fname + " " + train_predict + " " + train_error;
    const char* command_char = command.c_str();
    printf_s(command_char);
    printf_s("\n");
    FILE* pipe = _popen(command_char, "r");
    if (!pipe) {
        printf_s("Ошибка открытия потока для выполнения скрипта Python\n");
        exit(1);
    }
    char buffer[128];
    while (!feof(pipe)) {
        std::string result = "";
        std::unique_lock<std::mutex> ulm2(mtx2);
        dataWritten = false;
        if (fgets(buffer, 128, pipe) != NULL)
        {
            dataWritten = false;
            result = buffer;
            result.pop_back();
            std::cout << result << std::endl;
        }
        // Закрытие потока
        if (result == "Model is complete Loading")
        {
            printf_s("Поток разблокирован\n");
            dataWritten = true;
            cv.notify_one();
            dataWritten2 = false;
            cv2.wait(ulm2, ret2);

        }
        else if (result == "Model is Complete session")
        {
            printf_s("Поток разблокирован\n");

            dataWritten = true;
            cv.notify_one();
            dataWritten2 = false;
            cv2.wait(ulm2, ret2);
        }
        else
        {
            dataWritten = false;
        }
    }
    _pclose(pipe);
}
void TrainPyMLModel(std::string ModelFolder)
{
    //Здесь обучается модель
    dataWritten = false;
    printf_s("Сработал TrainPyMLModel\n");
    std::string test_fname = "testevent.txt";
    std::string train_fname = "vation.txt";
    std::string location = GeTNameDir();

    std::string command = "python -u " + location + char(92) + "model.py" + " " + ModelFolder + " " + test_fname + " " + train_fname;
    const char* command_char = command.c_str();
    printf_s(command_char);
    printf_s("\n");
    FILE* pipe = _popen(command_char, "r");
    if (!pipe) {
        printf_s("Ошибка открытия потока для выполнения скрипта Python\n");
        exit(1);
    }
    char buffer[128];
    while (!feof(pipe)) {
        std::string result = "";
        std::unique_lock<std::mutex> ulm2(mtx2);

        dataWritten = false;

        if (fgets(buffer, 128, pipe) != NULL)
        {
            dataWritten = false;
            result = buffer;
            result.pop_back();
            std::cout << result << std::endl;
        }
        // Закрытие потока
        if (result == "Model is complete Training")
        {
            printf_s("Поток разблокирован\n");
            dataWritten = true;
            cv.notify_one();
            dataWritten2 = false;
            cv2.wait(ulm2, ret2);

        }
        else if (result == "Model is Complete session")
        {
            printf_s("Поток разблокирован\n");

            dataWritten = true;
            cv.notify_one();
            dataWritten2 = false;
            cv2.wait(ulm2, ret2);
        }
        else
        {
            dataWritten = false;
        }
    }
    _pclose(pipe);
}
int writeCollectionEventID(Eventid& writeEvent1, Eventid& writeEvent2)
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::unique_lock<std::mutex> ulm(mtx);
    cv.wait(ulm, ret);

    printf_s("Начался сбор данных\n");
    dataWritten2 = false;

    int count = 0;
    std::string location = GeTNameDir(); //Требует доработки

    std::ofstream writelog;//, fstream::in | fstream::out | fstream::app);
    writelog.open("testevent.txt", std::ios_base::trunc);
    if (writeEvent1.getrecordNumber_() == 0)
    {
        collect(writeEvent1);

        // writelog << writeEvent1.geteventid_() << "\t" << writeEvent1.getrecordNumber_() << "\n";// << "\t" << writeEvent1.geteventType_() << "\t" << writeEvent1.getdateWrite_() << endl;
    }
    while (count < 13)
    {
        if (collect(writeEvent1, writeEvent2) != 0)
        {
            try {
                writelog << writeEvent1.geteventid_() << "\t" << writeEvent1.getrecordNumber_() << "\t" << writeEvent1.geteventType_() << "\t" << writeEvent1.getdateWrite_() << "\n";
                ++count;
            }
            catch (std::exception& ex)
            {
                printf_s(ex.what());
                printf_s("Произошла ошибка при сборе данных");
                exit(1);
            }
        }
    }
    writelog.close();
    printf_s("Сбор данных завершен!\n");
    dataWritten2 = true;
    cv2.notify_one();
    dataWritten = false;
    //cv.wait(ulm, ret);

    return 0;
}

int main(int argv, char* argc)
{
    setlocale(LC_CTYPE, "Russian");

    Decision_NN desNN; //Решение по продолжеению обучение модели
    TrainingNNinfo PrevTrain; // Хранится существующий дамп прошлой тренировки

    std::function<void(const std::string&) > selectedFunction; //Полиморфная обертка функции в конце потреуется либо для Training,либо для Load
    std::string ModelFolder = "";
    switch (int result_check = parsInterruption(PrevTrain))
    {
    case FOUND_INFO:
    {
        std::string push_string_name1 = "Продолжить сбор данных?:";
        desNN.NeedsTrains(push_string_name1);
        if (desNN.getdecisionAnswer() == YES)
        {
            printf_s("Выбран пункт продолжения обучения модели!\n");
            desNN.set_dataDecision(PrevTrain.GetData());
            desNN.set_elapstimeDecision(PrevTrain.GetelapsTime());
            desNN.set_collectedtime(PrevTrain.GetTimeCollected());
        }
        else
        {
            printf_s("Не продалжать сбор данных!\n");
            std::string push_string_name2 = "Нужно ли заново обучить модель?";
            desNN.NeedsTrains(push_string_name2);
            if (desNN.getdecisionAnswer() == YES)
            {
                unsigned int time_to_col = time_collect();
                desNN.set_elapstimeDecision(time_to_col);
                desNN.set_dataDecision(times_date_now());
                desNN.set_collectedtime(0);
                desNN.printdes();
            }
        }
        break;
    }
    case CLEAR_INFO:
    {
        std::string push_string_name3 = "Нужно ли обучить модель?";
        desNN.NeedsTrains(push_string_name3);
        if (desNN.getdecisionAnswer() == YES)
        {
            unsigned int time_to_col = time_collect();
            desNN.set_elapstimeDecision(time_to_col);
            desNN.set_dataDecision(times_date_now());
            desNN.set_collectedtime(0);
            desNN.printdes();
        }
        break;
    }
    }
    //Сбор event id 
    if (desNN.getdecisionAnswer() == YES)
    {
        Eventid eventid;
        Eventid eventid2;

        time_t start_count, end_count;
        time(&start_count);
        time(&end_count);
        unsigned int past_time = 0;


        std::string location = GeTNameDir() + "\\" + "InfoTraining" + "\\" + "training.json";
        std::ofstream writelearn(location, std::ios::trunc);

        TrainingNNinfo info_collect;
        json JsonNNinfo;

        info_collect.setData(desNN.get_dataDecision());
        auto ee = desNN.get_elapstimeDecision();
        info_collect.setelapsTime(ee);
        info_collect.setTimeCollected(past_time + desNN.get_collectedtime());

        info_collect.toJson(JsonNNinfo);
        writelearn << JsonNNinfo.dump(4);

        std::ofstream file("vation.txt", std::ios::app);
        std::fstream readfile("loggers.txt", std::fstream::in | std::fstream::out | std::fstream::app);

        //readfile.write((char*)&eventid,sizeof(Eventid)); //Для бинарной записи

        collect(eventid);
        while (time(&end_count) < start_count + desNN.get_elapstimeDecision())
        {
            if (collect(eventid, eventid2) != 0)
            {
                try {
                    file << eventid.geteventid_() << std::endl;
                    readfile << eventid.geteventid_() << "\t" << eventid.getrecordNumber_() << "\t" << eventid.geteventType_() << "\t" << eventid.getdateWrite_() << std::endl;
                }
                catch (std::exception& ex)
                {
                    printf_s(ex.what());
                    printf_s("Произошла ошибка при сборе данных\n");
                    exit(1);
                }
            }
            if (end_count % 1800 == 0)
            {
                past_time = (unsigned int)(end_count - start_count);

                writelearn.close();
                writelearn.open(location, std::ios::trunc);
                info_collect.setTimeCollected(past_time + desNN.get_collectedtime());
                info_collect.toJson(JsonNNinfo);
                writelearn << JsonNNinfo.dump(4);
            }
        }
        file.close();
        readfile.close();
        writelearn.close();
        printf_s("Сбор данных завершен!\n");

        ModelFolder = GeTNameDir();
        selectedFunction = TrainPyMLModel;
    }

    else if (desNN.getdecisionAnswer() == NO)
    {
        //здесь реализую загрузку модели
        printf_s("Warning! Название папки должно быть из латиницы\n");
        printf_s("Папка c обученной моделью должна распологаться по этому адресу:\n");
        std::cout << GeTNameDir() << std::endl;
        printf_s("Введите название папки где расположена модель, либо укажите иной путь расположения модели:");
        std::cin >> ModelFolder;
        std::string python_ml_models = "model.py";
        while (!(dirExists(ModelFolder)))
        {
            printf_s("Error! Путь не был обнаружен, повторите ввод:");
            std::cin >> ModelFolder;
        }
        printf_s("Путь обнаружен!\n");
        selectedFunction = LoadPyMLModel;

    }
    desNN.NeedsTrains("Начать обнаружение аномалий?");

    if (desNN.getdecisionAnswer() == YES)
    {
        Eventid writeEvent1;
        Eventid writeEvent2;
        std::thread pythonThread(OpenSourceModel, selectedFunction, std::ref(ModelFolder));
        while (true)
        {
            std::thread writenThread(writeCollectionEventID, std::ref(writeEvent1), std::ref(writeEvent2));

            writenThread.join();
        }
        pythonThread.join();
    }
    else if (desNN.getdecisionAnswer() == NO)
    {
        printf_s("Выход из программы!\n");
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}
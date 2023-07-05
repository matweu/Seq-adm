#include "SeqAdmfunc.h"

int yes_or_no(std::string printstring)
{
    std::string massWords1[] = { "yes","Yes","YEs","YES","yEs","yES","yeS","1" };
    std::string massWords2[] = { "NO","No","nO","no","0" };
    std::string answer;
    int valueAnswer = NOINFO;

    do
    {
        std::cout << printstring << std::endl;
        std::cout << "Введите yes или no:";
        std::cin >> answer;

        for (int i = 0; i < (sizeof(massWords1) / sizeof(massWords1[0])); i++)
        {
            if (answer == massWords1[i])
            {
                valueAnswer = YES;
            }
        }
        for (int i = 0; i < (sizeof(massWords2) / sizeof(massWords2[0])); i++)
        {
            if (answer == massWords2[i])
            {
                valueAnswer = NO;
            }
        }
        if (valueAnswer != NO && valueAnswer != YES)
        {
            std::cout << "Ответ не ясен,повторите ввод:\n";

        }


    } while (valueAnswer != NO && valueAnswer != YES);

    return valueAnswer;
}

bool NeedsTrainingModel(std::string printstring)
{
    std::string massWordsModels1[] = { "yes","Yes","YEs","YES","yEs","yES","yeS","1" };
    std::string massWordsModels2[] = { "NO","No","nO","no","0" };
    std::string answerTrainingModel;
    int valueAnswer = NoInfoTraining;

    do
    {
        std::cout << printstring << std::endl;
        std::cout << "Введите yes или no:";
        std::cin >> answerTrainingModel;

        for (int i = 0; i < (sizeof(massWordsModels1) / sizeof(massWordsModels1[0])); i++)
        {
            if (answerTrainingModel == massWordsModels1[i])
            {
                valueAnswer = YesNeedTrainModel;
            }
        }
        for (int i = 0; i < (sizeof(massWordsModels2) / sizeof(massWordsModels2[0])); i++)
        {
            if (answerTrainingModel == massWordsModels2[i])
            {
                valueAnswer = NoNeedTrainModel;
            }
        }
        if (valueAnswer != NoNeedTrainModel && valueAnswer != YesNeedTrainModel)
        {
            std::cout << "Ответ не ясен,повторите ввод:\n";

        }


    } while (valueAnswer != NoNeedTrainModel && valueAnswer != YesNeedTrainModel);

    return valueAnswer;
}

std::string GeTNameDir()
{
    std::string name;
    char corrent_work_dir[FILENAME_MAX];
    name = _getcwd(corrent_work_dir, sizeof(corrent_work_dir));
    if(name.size()!=0){ return name; }
    else {exit(1);}
}

void UNIXTimeToDate(char* buf, int unixtime)
{
    time_t t = unixtime;
    struct tm* tm = localtime(&t);
    char tmp[20];
    strftime(tmp, sizeof(tmp), "%d.%m.%Y %H:%M:%S", tm);
    strcpy(buf, tmp);
    return;
}

int parsInterruption(TrainingNNinfo& PrevTrain)
{
    std::string location = GeTNameDir() + "\\" + "InfoTraining" + "\\" + "training.json";
    std::ifstream checklearn(location);
    if (checklearn.is_open())
    {
        printf_s("Waning! Была обнаружена прерванная попытка сбора информации.\n");
        try
        {
            json readjsonNNinf;
            checklearn >> readjsonNNinf;
            PrevTrain.fromJson(readjsonNNinf);
            PrevTrain.PrintInfo();
            return FOUND_INFO;
        }
        catch (const std::exception& jsonexcept)
        {
            printf_s("Код ошибки:\n%s\n", jsonexcept.what());
            printf_s("Файл поврежден, не удалось восстановить log информации.\n");
            return CLEAR_INFO;
        }
        checklearn.close();

    }
    else
    {
        printf_s("Прерванных этапов сбора данных не найдено!\n");
    }
    
    return CLEAR_INFO;

}

std::string times_date_now()
{
    std::time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str(buffer);
    
    return str;

}

unsigned int time_collect()
{
    try {
        std::string collect_days_string;
        bool Nodigits = false;
        unsigned int time_interval;
        int collect_day = 0;
        printf_s("Введите количество дней,на котором будет проходить сбор [0,500]:");
        do
        {
            std::cin >> collect_days_string;
            Nodigits = false;
            for (const char c : collect_days_string)
            {
                if (!isdigit(c))
                {
                    Nodigits = true;
                    break;
                }
            }
            if (Nodigits == false)
            {
                collect_day = stoi(collect_days_string);

            }
            if (!(collect_day > 0 && collect_day < 500))
            {
                Nodigits = true;
                collect_day = 0;
            }
            if (Nodigits == true)
            {
                printf_s("Ввод некорректен, повторите попытку:");

            }
        } while (Nodigits == true);
        time_interval = collect_day * 86400; //Перевод в UNIX дату
        return time_interval;

    }
    catch (std::exception& writenumeric)
    {
        std::cout << writenumeric.what() << std::endl;
        printf_s("Ошибка в воде данных!\n");
        exit(1);

    }
}

bool dirExists( std::string& dirName_in)
{
    
    DWORD fileAttributes = GetFileAttributesA(dirName_in.c_str());

    if (fileAttributes != INVALID_FILE_ATTRIBUTES) {
        char fullPath[MAX_PATH];
        DWORD fullPathSize = GetFullPathNameA(dirName_in.c_str(), MAX_PATH, fullPath, nullptr);
        if (fullPathSize != 0) {
            std::string fullPathString(fullPath);
            dirName_in = std::move(fullPathString);
            return true;
        }
    }
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) { return false; }
    
    //if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) { return true; }
    return false;
}

void WriteInfoTraining( TrainingNNinfo &NNinfo)//std::string data, unsigned int TimeCol, unsigned int elapstime)
{
    std::string writefolder = GeTNameDir() + "\\" + "InfoTraining";
    
    std::string location = writefolder + "\\" + "training.json";
    
    if (dirExists(writefolder))
    {
        std::ofstream writelearn(location, std::ios::trunc);

        //TrainingNNinfo NNinfo(data, TimeCol, elapstime);
        json jsonNNinfo;
        NNinfo.toJson(jsonNNinfo);
        writelearn << jsonNNinfo.dump(4);
        writelearn.close();
    }
    else
    {
        std::filesystem::create_directory(writefolder);
        std::ofstream writelearn(location, std::ios::trunc);

        //TrainingNNinfo NNinfo(data, TimeCol, elapstime);
        json jsonNNinfo;
        NNinfo.toJson(jsonNNinfo);
        writelearn << jsonNNinfo.dump(4);
        writelearn.close();
    }
    
}

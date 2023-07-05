#pragma once
#pragma warning(disable : 4996)

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <execution>
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include <string.h>
#include "dumpJson.h"
#include <filesystem>

bool NeedsTrainingModel(std::string printstring);

int yes_or_no(std::string printstring);

int TimeInterval();

std::string GeTNameDir();

void UNIXTimeToDate(char* buf, int unixtime);

int parsInterruption(TrainingNNinfo& PrevTrain);

std::string times_date_now();

unsigned int time_collect();

bool dirExists(std::string& dirName_in);

void WriteInfoTraining(TrainingNNinfo& NNinfo);//std::string data, unsigned int TimeCol, unsigned int elapstime);


enum TypeTrainingModels
{
    NoNeedTrainModel = 0,
    YesNeedTrainModel = 1,
    NoInfoTraining = 2
};

enum ReadFiles
{
    ReadClose = 0,
    ReadOk = 1,
};


enum TYPECHECKLEARN
{
    EROR_FOUND_INFO = 0,
    FOUND_INFO = 1,
    CLEAR_INFO = 2

};

enum decision_enum
{
    NO=0,
    YES=1,
    NOINFO=2
};


#include "CollectorEvent.h"

void Eventid::setInfoID(long int eventid_, long int recordNumber_, int eventType_, std::string dateWrite_)
{
    this->eventid_ = eventid_;
    this->recordNumber_ = recordNumber_;
    this->eventType_ = eventType_;
    this->dateWrite_ = dateWrite_;
}

void Eventid::PrintId()
{
    std::cout << dateWrite_ << " ";
    printf("ID: %lu, Тип: %lu, RecordNumber: %lu\n", eventid_, eventType_, recordNumber_);

}
long int Eventid::geteventid_()
{
    return eventid_;
}
long int Eventid::getrecordNumber_()
{
    return recordNumber_;
}
int Eventid::geteventType_()
{
    return eventType_;
}
std::string Eventid::getdateWrite_()
{
    return dateWrite_;
}
Eventid& Eventid::operator=(Eventid& other)
{
    this->eventid_ = other.eventid_;
    this->recordNumber_ = other.recordNumber_;
    this->eventType_ = other.eventType_;
    this->dateWrite_ = other.dateWrite_;
    return *this;
}

void writeEvent(Eventid& eventid)
{
    std::ofstream file("vation.txt", std::ios::app);
    file << eventid.geteventid_() << std::endl;
    file.close();

}

void writelog(Eventid& eventid)
{
    std::fstream readfile("loggers.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    //readfile.write((char*)&eventid,sizeof(Eventid)); //Для бинарной записи
    readfile << eventid.geteventid_() << "\t" << eventid.getrecordNumber_() << "\t" << eventid.geteventType_() << "\t" << eventid.getdateWrite_() << std::endl;
    readfile.close();
}

Eventid collect(Eventid& eventid)
{
    LPCWSTR pwsLogName = LPCWSTR(L"Security");
    HANDLE hEventLog = OpenEventLog(NULL, pwsLogName);
    if (hEventLog == NULL)
    {
        std::cout << "Не удалось открыть журнал событий. Код ошибки: " << GetLastError() << std::endl;
        exit(1);
    }

    const int bufferSize = 500000; // Установка размера буфера чтения записей

    BYTE* pBuffer = new BYTE[bufferSize];
    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;
    //DWORD recordCount = 0;

    while (ReadEventLog(hEventLog, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
        0, pBuffer, bufferSize, &bytesRead, &totalBytesRead))
    {
        if (bytesRead == 0) { break; }

        EVENTLOGRECORD* pEvent = (EVENTLOGRECORD*)pBuffer;
        while ((BYTE*)pEvent < pBuffer + bytesRead)
        {

            char date[50];
            char* stst = new char[50];
            UNIXTimeToDate(stst, pEvent->TimeGenerated);
            sprintf(date, stst);
            //cout << date<<" ";
           // printf("ID: %lu, Тип: %lu, RecordNumber: %lu\n",pEvent->EventID & 0xFFFF, pEvent->EventType,pEvent->RecordNumber);
            eventid.setInfoID((pEvent->EventID & 0xFFFF), pEvent->RecordNumber, pEvent->EventType, date);
            eventid.PrintId();
            pEvent = (EVENTLOGRECORD*)((BYTE*)pEvent + pEvent->Length);
            //recordCount++;
            // cout << recordCount << endl;
            delete[] stst;
            break;
        }
        break;
    }
    delete[] pBuffer;
    CloseEventLog(hEventLog);
    return eventid;
}

bool collect(Eventid& eventid, Eventid& eventid2)
{
    LPCWSTR pwsLogName = LPCWSTR(L"Security");
    HANDLE hEventLog = OpenEventLog(NULL, pwsLogName);
    if (hEventLog == NULL)
    {
        std::cout << "Не удалось открыть журнал событий. Код ошибки: " << GetLastError() << std::endl;
        exit(1);
    }

    const int bufferSize = 500000; // Установка размера буфера чтения записей

    BYTE* pBuffer = new BYTE[bufferSize];
    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;
    DWORD recordCount = 0;
    while (ReadEventLog(hEventLog, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
        0, pBuffer, bufferSize, &bytesRead, &totalBytesRead))
    {
        if (bytesRead == 0) { break; }

        EVENTLOGRECORD* pEvent = (EVENTLOGRECORD*)pBuffer;
        while ((BYTE*)pEvent < pBuffer + bytesRead)
        {
            char date2[50];
            char stst[50];
            UNIXTimeToDate(stst, pEvent->TimeGenerated);
            sprintf(date2, stst);
            eventid2.setInfoID((pEvent->EventID & 0xFFFF), pEvent->RecordNumber, pEvent->EventType, date2);
            auto RecordNumber1 = eventid.getrecordNumber_();
            auto RecordNumber2 = eventid2.getrecordNumber_();
            if (RecordNumber2 == RecordNumber1 + 1)
            {
                eventid = eventid2;
                eventid2.PrintId();
                delete[] pBuffer;
                CloseEventLog(hEventLog);
                return 1;
            }
            if (RecordNumber2 > RecordNumber1)
            {
                pEvent = (EVENTLOGRECORD*)((BYTE*)pEvent + pEvent->Length);
            }
            else
            {
                delete[] pBuffer;
                CloseEventLog(hEventLog);
            }
        }
    }
    return 0;
}
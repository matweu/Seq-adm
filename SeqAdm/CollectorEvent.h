#pragma once
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "SeqAdmfunc.h"
class Eventid
{
public:

    Eventid() {
        recordNumber_ = 0;
    }
    Eventid(long int eventid_, long int recordNumber_, int eventType_, std::string dateWrite_)
    {
        this->eventid_ = eventid_;
        this->recordNumber_ = recordNumber_;
        this->eventType_ = eventType_;
        this->dateWrite_ = dateWrite_;

    }
    void setInfoID(long int eventid_, long int recordNumber_, int eventType_, std::string dateWrite_);
 
    void PrintId();
   
    //~Eventid() {}

    long int geteventid_();
   
    long int getrecordNumber_();
   
    int geteventType_();
   
    std::string getdateWrite_();
    
    Eventid& operator=(Eventid& other);
    
private:
    long int eventid_;
    long int recordNumber_;
    int eventType_;
    std::string dateWrite_;

};

void writeEvent(Eventid& eventid);

void writelog(Eventid& eventid);

Eventid collect(Eventid& eventid);

bool collect(Eventid& eventid, Eventid& eventid2);
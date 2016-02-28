#pragma once
#include "string"
#include "string.h"
#include <libxml++/libxml++.h>
#include <exception>
#include <boost/scoped_ptr.hpp>

class SpoorInformationAlreadyAvailableException: public std::exception
{
private:
    #define maxMsgLength 100
    char m_message[maxMsgLength+1];
public:
    SpoorInformationAlreadyAvailableException()
    {
        strncpy(m_message,"This data is already defined",maxMsgLength);
    }
    SpoorInformationAlreadyAvailableException(const char *message)
    {
        strncpy(m_message,message,maxMsgLength);
    }
    virtual const char* what() const throw()
    {
        return m_message;
    }
};


class SpoorInformatie
{
public:
    SpoorInformatie();
    ~SpoorInformatie();
    void AddStation(std::string station);
    void AddSpoor(std::string station, std::string spoor);
    void AddBlok(std::string station, std::string spoor, int blok);
    bool IsBlokAanwezig(std::string station, std::string spoor, int blok);
    int GetStopBlok(std::string station, std::string spoor, int richting);
    int GetTreinBlok(std::string station, std::string spoor, int richting);
    
    void AddStopBlok(std::string station, std::string spoor, int blok, int richting);
    void SaveData(std::string filename);
    void LoadData(std::string filename);
private:
    boost::scoped_ptr<xmlpp::DomParser> m_stationInfo;
    std::string GetTextFromNodeSet(xmlpp::Node *node);
    
};


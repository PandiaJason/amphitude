#ifndef UPNPMAPPER_H
#define UPNPMAPPER_H

#include <string>
#include <iostream>

class UPnPMapper {
public:
    UPnPMapper();
    ~UPnPMapper();

    bool init();
    bool addPortMapping(int port, const std::string& protocol = "TCP");
    bool removePortMapping(int port, const std::string& protocol = "TCP");
    std::string getExternalIP();

private:
    struct IGDDatas {
        char curExternalIPAddress[16];
        std::string controlURL;
        std::string serviceType;
        std::string urls_controlURL;
        std::string urls_ipCondescURL;
        std::string data_first_file;
    } igdData;

    bool initialized = false;
    std::string lanAddress;
};

#endif

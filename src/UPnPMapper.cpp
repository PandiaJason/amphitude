#include "UPnPMapper.h"
#include "UPnPMapper.h"
#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>
#include <cstring>

UPnPMapper::UPnPMapper() {}

UPnPMapper::~UPnPMapper() {}

bool UPnPMapper::init() {
    int error = 0;
    struct UPNPDev* devlist = upnpDiscover(2000, NULL, NULL, 0, 0, 2, &error);
    if (!devlist) {
        std::cerr << "UPnP: No devices found." << std::endl;
        return false;
    }

    struct UPNPUrls urls;
    struct IGDdatas data;
    char lanAddr[64];
    char wanAddr[64];

    int status = UPNP_GetValidIGD(devlist, &urls, &data, lanAddr, sizeof(lanAddr), wanAddr, sizeof(wanAddr));
    
    if (status == 1 || status == 2) {
        std::cout << "UPnP: Found IGD at " << urls.controlURL << std::endl;
        std::cout << "UPnP: Local LAN IP: " << lanAddr << std::endl;
        
        igdData.controlURL = urls.controlURL;
        igdData.serviceType = data.first.servicetype;
        lanAddress = lanAddr;
        initialized = true;
        
        // Free devlist but keep urls? No, GetValidIGD copies what we need? 
        // Actually we need to copy the strings if we want to keep them, 
        // but for simplicity let's assume we use them immediately or copy them.
        // Wait, UPNPUrls contains pointers. We need to be careful.
        // For this simple implementation, let's just keep the struct copies if possible
        // or re-discover if needed. But re-discovering is slow.
        // Let's copy the strings we need.
        
        // Actually, miniupnpc usage usually involves keeping 'urls' valid?
        // No, FreeUPNPUrls(&urls) is needed later.
        // Let's just store the strings we need.
        
        // Correction: We need to keep 'urls' valid or copy the strings.
        // Let's assume we just use the strings we copied to igdData.
        // But wait, UPNP_AddPortMapping needs the URL.
        
        // To be safe and simple, we will store the necessary strings.
    } else {
        std::cerr << "UPnP: No valid IGD found." << std::endl;
        freeUPNPDevlist(devlist);
        return false;
    }
    
    // We should free the devlist
    freeUPNPDevlist(devlist);
    return true;
}

bool UPnPMapper::addPortMapping(int port, const std::string& protocol) {
    if (!initialized) return false;

    std::string portStr = std::to_string(port);
    std::string desc = "Amphitude Game";
    
    int r = UPNP_AddPortMapping(igdData.controlURL.c_str(), 
                                igdData.serviceType.c_str(), 
                                portStr.c_str(), 
                                portStr.c_str(), 
                                lanAddress.c_str(), 
                                desc.c_str(), 
                                protocol.c_str(), 
                                NULL, "0");

    if (r == UPNPCOMMAND_SUCCESS) {
        std::cout << "UPnP: Port " << port << " mapped successfully!" << std::endl;
        return true;
    } else {
        std::cerr << "UPnP: AddPortMapping failed: " << strupnperror(r) << std::endl;
        return false;
    }
}

bool UPnPMapper::removePortMapping(int port, const std::string& protocol) {
    if (!initialized) return false;

    std::string portStr = std::to_string(port);
    
    int r = UPNP_DeletePortMapping(igdData.controlURL.c_str(), 
                                   igdData.serviceType.c_str(), 
                                   portStr.c_str(), 
                                   protocol.c_str(), 
                                   NULL);

    if (r == UPNPCOMMAND_SUCCESS) {
        std::cout << "UPnP: Port " << port << " unmapped." << std::endl;
        return true;
    } else {
        std::cerr << "UPnP: DeletePortMapping failed: " << strupnperror(r) << std::endl;
        return false;
    }
}

std::string UPnPMapper::getExternalIP() {
    if (!initialized) return "";
    char externalIP[40];
    int r = UPNP_GetExternalIPAddress(igdData.controlURL.c_str(), 
                                      igdData.serviceType.c_str(), 
                                      externalIP);
    if (r == UPNPCOMMAND_SUCCESS) {
        return std::string(externalIP);
    }
    return "";
}

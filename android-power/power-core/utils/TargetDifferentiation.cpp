/******************************************************************************
  @file    TargetDifferentiation.cpp
  @brief   Implementation for target differentiation for feature obfuscation

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdlib>
#include <fstream>
#include <vector>
#include <inc/PowerOptDebugLog.h>
#include <cutils/properties.h>
#include "TargetDifferentiation.h"

#define LOG_TAG_TARGET_DIFFERENTIATION "ANDR-PWR-Target-Differentiation"
#define ASCII_CHART_SIZE 256
#define BASE64_CHART_SIZE 64
#define BIT_REPR_8 8
#define BIT_REPR_6 6
#define ENCODE_REPR 4
#define HEX_63 0x3F
#define HEX_255 0xFF
typedef unsigned char uchar;
#define BASE_64_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

using namespace std;

std::string TargetDifferentiation::encode(const std::string in) {

    std::string encodedString;

    int asciiEncode=0, decConv=-6;
    for (uchar c : in) {
        asciiEncode = (asciiEncode<<BIT_REPR_8) + c;//Representing each character as 8 bit binary
        decConv += BIT_REPR_8;
        while (decConv>=0) {
            encodedString.push_back(BASE_64_CHARS[(asciiEncode>>decConv)&HEX_63]);// encoding from last bit by representing them with 6 bits and & with 0x3F (hex of 63)
            decConv-=BIT_REPR_6;// representing in 6 bits
        }
    }
    if (decConv>-BIT_REPR_6)
       encodedString.push_back(BASE_64_CHARS[((asciiEncode<<BIT_REPR_8)>>(decConv+8))&HEX_63]);   //Check for last character

    while (encodedString.size()%ENCODE_REPR)
       encodedString.push_back('=');

    std::reverse(encodedString.begin(), encodedString.end());//Reversal of the encoded string

    return encodedString;
}

std::string TargetDifferentiation::decode(const std::string in){

    std::string decodedString;

    std::string out = in;
    std::reverse(out.begin(), out.end());

    std::vector<int> T(ASCII_CHART_SIZE,-1);
    for (int i=0; i<BASE64_CHART_SIZE; i++)
       T[BASE_64_CHARS[i]] = i;

    int asciiEncode=0, decConv=-BIT_REPR_8;
    for (uchar c : out) {
        if (T[c] == -1)
          break;
        asciiEncode = (asciiEncode<<BIT_REPR_6) + T[c];
        decConv += BIT_REPR_6;
        if (decConv>=0) {
            decodedString.push_back(char((asciiEncode>>decConv)&HEX_255));
            decConv-=BIT_REPR_8;
        }
    }
    return decodedString;
}

static inline unsigned long GetInt(string buf) {
    buf = TargetDifferentiation::decode(buf);
    unsigned long num = strtol(buf.c_str(), NULL, 10);
    return num;
}

static int GetSocId() {
    ifstream fd;
    string strData;
    int soc_id = -1;
    /*socbuf = /sys/devices/soc0/soc_id*/
    string socbuf = "kl2Xj92cvAzYvN3LzV2YpZXZk9yc5N3L";
    socbuf = TargetDifferentiation::decode(socbuf);
    fd.open(socbuf, ios::in);
    if (!fd.is_open()) {
        SLOGE("Unable to open file");
        return -1;
    }

    getline(fd, strData);
    if (strData.length() != 0) {
        soc_id = stoi(strData);
    } else {
        SLOGE("id is null");
    }
    fd.close();

    return soc_id;
}

int TargetDifferentiation::GetSocName(char* name) {
    int ret = property_get("ro.vendor.qti.soc_name", name, "Undefined");
    return ret;
}

static unsigned long GetMaxGpuFreq() {
    string strData;
    ifstream fd;
    unsigned long max = 0;
    /*availbuf = /sys/class/kgsl/kgsl-3d0/devfreq/available_frequencies*/
    string availbuf = "zVWaj5WZ1FXZyZ2XlxmYhxWahZXYvEXZyZmdlR2LwQ2Mtw2cnt2LsN3Zr9yczFGbj9yc5N3L";
    availbuf = TargetDifferentiation::decode(availbuf);

    fd.open(availbuf, ios::in);
    if (!fd.is_open()) {
        SLOGE("Unable to open file");
        return -1;
    }

    while (getline(fd, strData, ' ')) {
        unsigned long i = stoul(strData);
        if (i >= max) {
            max = i;
        }
    }
    fd.close();
    return max;
}

bool TargetDifferentiation::IsFeatureEnabled(string feature) {
    unsigned int numentries;
    bool flag = false;
    unsigned long soc_id = GetSocId();
    unsigned long max_gpu_freq = GetMaxGpuFreq();

    //lito 400
    string socidbuf1 = "wADN";
    unsigned long socid1 = GetInt(socidbuf1);

    //lagoon 434
    string socidbuf2 = "0MDN";
    unsigned long socid2 = GetInt(socidbuf2);

    //lagoon 459
    string socidbuf3 = "5UDN";
    unsigned long socid3 = GetInt(socidbuf3);

    //fraser 476
    string socidbuf4 = "2cDN";
    unsigned long socid4 = GetInt(socidbuf4);

    //cedros 450
    string socidbuf5 = "wUDN";
    unsigned long socid5 = GetInt(socidbuf5);

    //mannar 454
    string socidbuf6 = "0UDN";
    unsigned long socid6 = GetInt(socidbuf6);

    //lahaina 415
    string socidbuf7 = "1EDN";
    unsigned long socid7 = GetInt(socidbuf7);

    //waipio 456
    string socidbuf8 = "2UDN";
    unsigned long socid8 = GetInt(socidbuf8);

    //waipio 457
    string socidbuf13 = "3UDN";
    unsigned long socid13 = GetInt(socidbuf13);

    //waipio 482
    string socidbuf21 = "ygDN";
    unsigned long socid21 = GetInt(socidbuf21);

    //yupik 475
    string socidbuf9 = "1cDN";
    unsigned long socid9 = GetInt(socidbuf9);

    //yupik 499
    string socidbuf10 = "5kDN";
    unsigned long socid10 = GetInt(socidbuf10);

    //lahaina pro 439
    string socidbuf11 = "5MDN";
    unsigned long socid11 = GetInt(socidbuf11);

    //yupik 515
    string socidbuf12 = "1ETN";
    unsigned long socid12 = GetInt(socidbuf12);

    //alakai 552
    string socidbuf14 = "yUTN";
    unsigned long socid14 = GetInt(socidbuf14);

    //fillmore 506
    string socidbuf15 = "2ATN";
    unsigned long socid15 = GetInt(socidbuf15);

    //palima 530
    string socidbuf16 = "wMTN";
    unsigned long socid16 = GetInt(socidbuf16);

    //palima 540
    string socidbuf17 = "wQTN";
    unsigned long socid17 = GetInt(socidbuf17);

    //kailua 519
    string socidbuf18 = "5ETN";
    unsigned long socid18 = GetInt(socidbuf18);

    //kailua 536
    string socidbuf19 = "2MTN";
    unsigned long socid19 = GetInt(socidbuf19);

    //parrot 537
    string socidbuf20 = "3MTN";
    unsigned long socid20 = GetInt(socidbuf20);

    //ukee 591
    string socidbuf22 = "xkTN";
    unsigned long socid22 = GetInt(socidbuf22);

    //kailua 600
    string socidbuf23 = "wAjN";
    unsigned long socid23 = GetInt(socidbuf23);

    //kailua 601
    string socidbuf24 = "xAjN";
    unsigned long socid24 = GetInt(socidbuf24);

    //lanai 557
    string socidbuf25 = "3UTN";
    unsigned long socid25 = GetInt(socidbuf25);

    //lanai 577
    string socidbuf26 = "3cTN";
    unsigned long socid26 = GetInt(socidbuf26);

    //palawan 614
    string socidbuf27 = "0EjN";
    unsigned long socid27 = GetInt(socidbuf27);

    //palawan 632
    string socidbuf28 = "yMjN";
    unsigned long socid28 = GetInt(socidbuf28);

    //sun 618
    string socidbuf29 = "4EjN";
    unsigned long socid29 = GetInt(socidbuf29);

    //piti 623
    string socidbuf30 = "zIjN";
    unsigned long socid30 = GetInt(socidbuf30);

    //milos 657
    string socidbuf31 = "3UjN";
    unsigned long socid31 = GetInt(socidbuf31);

    //milos 658
    string socidbuf32 = "4UjN";
    unsigned long socid32 = GetInt(socidbuf32);

    //milos 636
    string socidbuf33 = "2MjN";
    unsigned long socid33 = GetInt(socidbuf33);

    //milos 640
    string socidbuf34 = "wQjN";
    unsigned long socid34 = GetInt(socidbuf34);

    //lanai game sku 682
    string socidbuf35 = "ygjN";
    unsigned long socid35 = GetInt(socidbuf35);

    //Netrani2.5 663
    string socidbuf36 = "zYjN";
    unsigned long socid36 = GetInt(socidbuf36);

    //lanai sku 696
    string socidbuf37 = "2kjN";
    unsigned long socid37 = GetInt(socidbuf37);

    //Bonito 655
    string socidbuf38 = "1UjN";
    unsigned long socid38 = GetInt(socidbuf38);

    if (feature == "VideoPowerOptFeature"){
            //Enable for lito
            if (soc_id == socid1) {
                flag = true;
            }
            //enable for lagoon
            else if (soc_id == socid2) {
                flag = true;
            }
            else if (soc_id == socid3) {
                flag = true;
            }
            //enable for fraser
            else if (soc_id == socid4) {
                flag = true;
            }
            //enable for cedros
            else if (soc_id == socid5) {
                flag = true;
            }//enable for mannar
            else if (soc_id == socid6) {
                flag = true;
            }//enable for lahaina
            else if (soc_id == socid7 || soc_id == socid11) {
                flag = true;
            }//enable for waipio
            else if (soc_id == socid8 || soc_id == socid13 || soc_id == socid14 || soc_id == socid21) {
                 flag = true;
            }//enable for palima
            else if (soc_id == socid16 || soc_id == socid17) {
                flag = true;
            }//enable for yupik
            else if (soc_id == socid9 || soc_id == socid10 || soc_id == socid12) {
                flag = true;
            }//enable for fillmore
            else if (soc_id == socid15) {
                flag = true;
            }//enable for kailua
            else if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
                flag = true;
            }//enable for parrot
            else if(soc_id == socid20 || soc_id == socid36){
                flag = true;
            }//enable for ukee
            else if(soc_id == socid22){
                flag = true;
            }//enable for lanai
            else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
                flag = true;
            }//enale for palawan
            else if(soc_id == socid27 || soc_id == socid28){
                flag = true;
            }//enale for sun
            else if(soc_id == socid29){
                flag = true;
            }
            else if(soc_id == socid30){
                flag = true;
            }//enable for milos
            else if(soc_id == socid31 || soc_id == socid32){
                flag = true;
            }//enale for Milos/Kimilos
            else if(soc_id == socid33 || soc_id == socid34){
                flag = true;
            }//enale for Bonito
            else if(soc_id == socid38){
                flag = true;
            }
    }
    if (feature == "PSMPowerOptFeature"){
        //enable for waipio
        if (soc_id == socid8 || soc_id == socid13 || soc_id == socid14 || soc_id == socid21) {
            flag = true;
        }//enable for palima
        else if (soc_id == socid16 || soc_id == socid17) {
            flag = true;
        }//enable for yupik
        else if (soc_id == socid9 || soc_id == socid10 || soc_id == socid12) {
            flag = true;
        }//enable for fillmore
        else if (soc_id == socid15) {
            flag = true;
        }//enable for kailua
        else if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
            flag = true;
        }//enable for parrot
        else if(soc_id == socid20){
            flag = true;
        }//enable for ukee
        else if(soc_id == socid22){
            flag = true;
        }//enable for lanai
        else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enale for sun
        else if(soc_id == socid29){
            flag = true;
        }//enable for milos
        else if(soc_id == socid31 || soc_id == socid32){
            flag = true;
        }//enale for Bonito
        else if(soc_id == socid38){
            flag = true;
        }
    }
    if (feature == "StandbyFeature"){
       if(soc_id == socid8 || soc_id == socid13 || soc_id == socid14 || soc_id == socid21) {
            flag = true;
        }//enable for fillmore
        else if (soc_id == socid15) {
            flag = true;
        }//enable for palima
        else if (soc_id == socid16 || soc_id == socid17) {
            flag = true;
        }//enable for kailua
        else if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
            flag = true;
        }//enable for parrot
        else if(soc_id == socid20){
            flag = true;
        }//enable for ukee
        else if(soc_id == socid22){
            flag = true;
        }//enable for lanai
        else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enable for milos
        else if(soc_id == socid31 || soc_id == socid32){
            flag = true;
        }
    }
    if (feature == "VendorScenarioPowerOptFeature"){
       if(soc_id == socid8 || soc_id == socid13 || soc_id == socid14 || soc_id == socid21) {
            flag = true;
        }//enable for fillmore
        else if (soc_id == socid15) {
            flag = true;
        }//enable for palima
        else if (soc_id == socid16 || soc_id == socid17) {
            flag = true;
        }//enable for kailua
        else if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
            flag = true;
        }//enable for parrot
        else if(soc_id == socid20){
            flag = true;
        }//enable for ukee
        else if(soc_id == socid22){
            flag = true;
        }//enable for lanai
        else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enale for sun
        else if(soc_id == socid29){
            flag = true;
        }//enable for milos
        else if(soc_id == socid31 || soc_id == socid32){
            flag = true;
        }
    }
    if (feature == "GamePowerOptFeature"){
       if(soc_id == socid8 || soc_id == socid13 || soc_id == socid14 || soc_id == socid21) {
            flag = true;
        }//enable for fillmore
        else if (soc_id == socid15) {
            flag = true;
        }//enable for palima
        else if (soc_id == socid16 || soc_id == socid17) {
            flag = true;
        }//enable for kailua
        else if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
            flag = true;
        }//enable for parrot
        else if(soc_id == socid20){
            flag = true;
        }//enable for ukee
        else if(soc_id == socid22){
            flag = true;
        }//enable for lanai
        else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enale for palawan
        else if(soc_id == socid27 || soc_id == socid28){
            flag = true;
        }//enale for sun
        else if(soc_id == socid29){
            flag = true;
        }//enale for Milos/Kimolos
        else if(soc_id == socid33 || soc_id == socid34){
            flag = true;
        }//enale for Bonito
        else if(soc_id == socid38){
            flag = true;
        }
    }
    if(feature == "AODPowerOptFeature"){
        //enable for kailua
        if(soc_id == socid18 || soc_id == socid19 || soc_id == socid23 || soc_id == socid24) {
            flag = true;
        }//enable for parrot
        else if(soc_id == socid20){
            flag = true;
        }//enable for ukee
        else if(soc_id == socid22){
            flag = true;
        }//enable for lanai
        else if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enable for milos
        else if(soc_id == socid31 || soc_id == socid32){
            flag = true;
        }
    }
    if(feature == "OffScreenPowerOptFeature"){
        //enable for lanai
        if(soc_id == socid25 || soc_id == socid26 || soc_id == socid35 || soc_id == socid37){
            flag = true;
        }//enale for palawan
        else if(soc_id == socid27 || soc_id == socid28){
            flag = true;
        }//enale for sun
        else if(soc_id == socid29){
            flag = true;
        }//enale for Milos/Kimilos
        else if(soc_id == socid33 || soc_id == socid34){
            flag = true;
        }//enale for Bonito
        else if(soc_id == socid38){
            flag = true;
        }
    }
    if(feature == "CameraPowerOptFeature"){
        //enale for sun
        if(soc_id == socid29){
            flag = true;
        }//enale for Bonito
        else if(soc_id == socid38){
            flag = true;
        }
    }
    return flag;
}

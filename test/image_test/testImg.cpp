/****************************************************************************
  FileName     [ testImg.cpp ]
  PackageName  [ image_test ]
  Synopsis     [ main function ]
  Author       [  ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include "nlohmann/json.hpp"
#include "LED_strip.h"

using json = nlohmann::json;
using namespace std;

#define alpha 1
#define TEST_INTERVAL  100000 // us
#define NUM_OF_LED 3
#define LED_0 300
#define LED_1 35
#define LED_2 35
const uint16_t numLEDs[NUM_OF_LED] = {LED_0, LED_1, LED_2};

string Dir = "./json/energy_final_json/";
const string Ending = ".json";
int id = 0, dataSize = 0;
char* RGB_data = NULL;
LED_Strip leds(NUM_OF_LED, numLEDs);


bool cmpEnding(const string& fullstr, const string& ending) { // compare ending of FileName
    if(fullstr.length() >= ending.length()) {
        return ( 0 == fullstr.compare(fullstr.length() - ending.length(), ending.length(), ending));
    }
    else return false;
}

void getFiles(const string&, vector<string>&);
void readFiles(const vector<string>&, vector<char*>&);



int main(int argc, char *argv[])
{
    if(argc == 3) {
        Dir = argv[2];
    }
    else if (argc != 2)
	{
		cerr << "[ERROR] Invalid parameter\n";
		cerr << "Usage: sudo ./testImg <strip ID> [JSON directory path]\n";
		return -1;
	}

    id = atoi(argv[1]);
    dataSize = numLEDs[id]*3;
    vector<string> FileNames;
    vector<char*>  RGBs;
    getFiles(Dir, FileNames);
    readFiles(FileNames, RGBs);
    for(size_t i = 0; i < RGBs.size(); ++i) {
        leds.sendToStrip(id, RGBs[i]);
        usleep(TEST_INTERVAL);
    }

    // turnoff
    char* tmp = 0;
    tmp = new char[dataSize];
    for(int j = 0; j < dataSize; ++j) tmp[j] = 0;
    leds.sendToStrip(id, tmp);
    delete[] tmp;
    cout << "done!" << endl;
}

void getFiles(const string& dir_path, vector<string>& files) {
    dirent** namelist;
    int count = 0;
    count = scandir(dir_path.c_str(), &namelist, NULL, alphasort);
    if(count == -1) {
        cerr << "scandir error! " << endl;
        return;
    }
    for(int i = 0; i < count; ++i) {
        if(cmpEnding(namelist[i]->d_name, Ending) && namelist[i]->d_type == DT_REG) {
            // cerr << namelist[i]->d_name << endl;
            files.push_back(namelist[i]->d_name);
        }
    }
    delete[] namelist;
}

void readFiles(const vector<string>& files, vector<char*>& RGB_datas) {
    cerr << "[INFO] Reading json..." << endl;
    string path;
    for(size_t i = 0; i < files.size(); ++i) {
        path = Dir;
        path.append(files[i]);
        ifstream infile(path);
        json RGB = json::parse(infile);
        char* data = new char[dataSize];
        for(int j = 0; j < dataSize; ++j) {
            double tmp = RGB[j];
            tmp *= alpha;
            data[j] = char(int(tmp));
        }
        RGB_datas.push_back(data);
    }
    cerr << "[INFO] Done Reading!!" << endl;
}
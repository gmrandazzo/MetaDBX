#include <algorithm>
#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <cctype>
#include <cstdarg>

#include "dymetdb.h"

// Remove chars from a string
void removeCharsFromString(std::string &str, std::string charsToRemove){
   for(int i = 0; i < charsToRemove.size(); ++i) {
      str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
   }
}

// Split a string
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

// Trim a string
std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

//missing string printf
//this is safe and convenient but not exactly efficient
inline std::string format(const char* fmt, ...){
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize){ //fail delete buffer and try again
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize+1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}

void PrintRes(std::vector<std::string> r)
{
  std::cout << "___________FOUND___________ " << std::endl;
  for(int i = 0; i < (int)r.size(); i++)
    std::cout << r[i] << std::endl;
  std::cout << "___________________________ " << std::endl;
}

int main(int argc, char **argv)
{
  if(argc == 13){
    /*Initialize the database*/
    DyMetDB *db = new DyMetDB;
    db->init(argv[11]);

    std::vector<std::string> adductlst;
    std::vector<std::string> mslst;
    std::vector<std::string> trlst;
    std::string inpstr;
    std::vector<std::string> r;

    std::string line;

    std::ifstream masstrlst(argv[1]);
    if(masstrlst.is_open()){
      while(getline(masstrlst, line)){
        std::vector<std::string> v = split(line, '_'); // Progenesis support
        if(v.size() == 2){
          trlst.push_back(v[0]);
          removeCharsFromString(v[1], "m/zn");
          mslst.push_back(v[1]);
        }
        else{
          v = split(line, '@'); // Mass hunter support
          if(v.size() == 2){
            trlst.push_back(v[0]);
            removeCharsFromString(v[1], "m/zn");
            mslst.push_back(v[1]);
          }
          else{
            continue;
          }
        }
      }
      masstrlst.close();
    }
    else std::cout << "Unable to open file m/z tr list" << std::endl;

    std::ifstream faddlst(argv[3]);

    if(faddlst.is_open()){
      while(getline(faddlst, line)){
        adductlst.push_back(trim(line));
      }
      faddlst.close();
    }
    else std::cout << "Unable to open adduct list" << std::endl;

    // Now for each mass search each adduct by running the query.
    // The results were collected into an output file.
    std::ofstream of;
    of.open(argv[12]);

    for(int j = 0; j < mslst.size(); j++){
      for(int i = 0; i < adductlst.size(); i++){
        //inpstr = "MS: 347.2219 error: 25ppm add: 1.0079; tR: 9.05 error: 5% init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.3099 vd: 0.375";
        inpstr = format("MS: %s error: %sppm add: %s; tR: %s error: %s%% init: %s final: %s tg: %s flow: %s vm: %s vd: %s", mslst[j].c_str(), argv[2], adductlst[i].c_str(), trlst[j].c_str(), argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]);
        //std::cout << "Searching for: " << inpstr << "\n" << std::endl;
        r = db->find(inpstr);
        if(r.size()> 0){
          //std::cout << "Results:" << std::endl;
          for(int k = 0; k < (int)r.size(); k++)
            of << mslst[i] << " and " << trlst[i] << " -> " << r[k] << std::endl;
          of << "END" << std::endl;
          PrintRes(r);
        }
        r.clear();
      }
    }
    delete db;
    of.close();

  }
  else{
    std::cout << format("\nUsage: %s <tr/mass list> <ppm error> <adduct mass list> <tr error> <gradient start> <gradient stop> <time gradient> <flow rate> <dead volume> <dwell volume> <CSV database>\n", argv[0]) << std::endl;
    std::cout << argv[0] << " was written by Giuseppe Marco Randazzo <gmrandazzo@gmail.com>\n" << std::endl;
  }
  return 0;
}

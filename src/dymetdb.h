#ifndef DYMETDB_H
#define DYMETDB_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

/*
 * Dynamic Metabolite Database
 *
 * This is a NoSQL database type
 * It consists into a key-value approach!
 * Each column of the table is indexed as an ordered list and related to a value
 * whic correspond to the metabolite database position.
 * This database can store/use a maximum of 2147483647 of molecules!
 */

inline bool isNumber(std::string str){
  double d;
  std::istringstream is(str);
  is >> d;
  return !is.fail() && is.eof();
}

inline double stod_(std::string str){
  return atof(str.c_str());
}

struct key_value
{
 //Struct initialization
 key_value(std::string &key_, int &value_): key(key_), value(value_){}
    //std::cout << key << " " << value << std::endl; }
 //Struct operators

 bool operator== (const std::string& key_) const
 {
   if(isNumber(key) == true){
     return std::fabs(stod_(key) - stod_(key_)) < std::numeric_limits<double>::epsilon();
   }
   else{
     //std::cout << " == " << key << key_ << std::endl;
     return key.compare(key_);
   }
 }

 bool operator< (const key_value& other) const
 {
   if(isNumber(key) == true){
     return stod_(key) - stod_(other.key);
   }
   else
    return key < other.key;
 }

 std::string key;
 int value;
};

struct DB
{
  std::vector<key_value*> collection;
};

/*limit of DyMetDB */
class DyMetDB
{
public:
  DyMetDB(){ nrow = ncol = 0; rtslope = 1.; rtintercept = 0.; }
  ~DyMetDB(){ for(int i = 0; i < (int)db.size(); i++) delete db[i]; };
  // init the db structure
  int init(std::string dbfile);
  /* find examples:
   * find(MS 233.2844 within error 5ppm at tR 10.25 within error 5%);
   * or
   * find(MS 233.2844 within error 5ppm);
   * or
   * find(tR 10.25 within error 5%);
   * or
   * find(Name Glucuronic acid);
   */
  std::vector<std::string> find(std::string qline);
  /* Sometimes retention times are not well aligned for column or tubing reasons.
   * Thus, dwell volume is not sufficient and standards are necessary to recalibrate the recalculation.
   * This method thus allow to add some corrective factors estimated from
   * already identified compounds. The retention time thus will be corrected according these parameters.
   */
  void setRTLinearAligner(std::string rttunfile, std::string qline);
  void setRTLinearAligner(double rtslope_, double rtintercept_) { rtslope = rtslope_; rtintercept = rtintercept_; } 
  /* Check if the database is empty */
  bool isEmpty(){ if(db.size() == 0) return true; else return false; }
  /* Insert a new record in the database */
  void insert(); // TO DEVELOP
  /* Update the database */
  void update(); // TO DEVELOP
  /* Remove a database record */
  void remove(); // TO DEVELOP
  /* Delete everything in the database */
  void clear();
private:
  // private methods
  std::vector<std::string> strsplit(const std::string &s, char delim){
    std::vector<std::string> list;
    std::stringstream ss(s);
    std::string item;
    while(getline(ss, item, delim)){
      if(!item.empty())
        list.push_back(item);
    }
    return list;
  }
  template <typename T> std::string NumberToString(T Number){
    std::ostringstream ss;
    ss << Number;
    return ss.str();
  }
  template <typename T> T StringToNumber(const std::string &Text){
    std::replace(Text.begin(), Text.end(), ',', '.');
    std::istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
  }
  //
  template< typename T > typename std::vector<T>::iterator
    insert_sorted(std::vector<T> & vec, T const& item){
      return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
  }

  std::vector<int> find_all_keys_id(std::vector<key_value> collection, std::string key);

  std::vector<std::string> parseqline(std::string qline);
  std::vector<std::string> getqline(std::vector<int> ids);
  // Apply the lss theory to calculate the logkw and s parameters
  double rtpred(double logkw, double s, double vm, double vd, double flux,
    double init_B, double final_B, double tg);
  double DaltonError(double mass, double ppm);
  double pround(double x, int precision);
  // private datas
  std::string dbfile;
  std::vector<DB*> db;
  std::vector<std::string> header;
  double rtslope, rtintercept;
  int nrow, ncol;
};

#endif

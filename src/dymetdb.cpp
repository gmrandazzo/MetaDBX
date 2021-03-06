#include "dymetdb.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>

int DyMetDB::init(std::string dbfile_)
{
  //std::cout << "Start Indexing " << dbfile_ << std::endl;
  dbfile = dbfile_;
  std::string line;
  std::ifstream fdb(dbfile.c_str());
  int prev_pos = 0;
  int tellg_pos = 0;
  bool firstline = true;
  if(fdb.is_open()){
    while(getline(fdb, line)){
      std::vector<std::string> v = strsplit(line, ';');
      prev_pos = tellg_pos;
      tellg_pos = fdb.tellg();
      if(firstline == true){
        firstline = false;
        //if(line.find("Name") != std::string::npos){
        // this is header;
        for(int i = 0; i < (int)v.size(); i++){
          db.push_back(new DB());
          header.push_back(v[i]);
          ncol = header.size();
        }
      }
      else{
        //std::cout << line << " " << db.size() << std::endl;
        //std::cout << line << std::endl;
        int db_size = (int)db.size();
        for(int i = 0; i < (int)v.size(); i++){
          //std::cout << v[i] << std::endl;
          if(i >= db_size){
            std::cout << "ERROR in line " << line << " columns > of db size"<< std::endl;
          }
          else{
            /*db[i]->collection.push_back(new key_value(v[i], prev_pos));*/
            insert_sorted(db[i]->collection,  new key_value(v[i], prev_pos));
          }
        }
      }
    }
    fdb.close();
  }
  else{
    // Unable to open database
    return -1;
  }

  /* SORT everything! when divide and conqueror algorithm is implemented
  std::cout << "SORT everything" << std::endl;
  for(int i = 0; i < (int)db.size(); i++){
    std::sort(db[i]->collection.begin(), db[i]->collection.end());
  }
  */

  /* Just for debug
  for(int i = 0; i < (int)db.size(); i++){
    for(int j = 0; j < db[i]->collection.size(); j++){
      std::cout << "key: "<<  db[i]->collection[j]->key << " value: " << db[i]->collection[j]->value << std::endl;
    }
  }*/

  //std::cout << "End Indexing" << std::endl;
  return 0;
}

void DyMetDB::clear()
{
  for(int i = 0; i < (int)db.size(); i++){
    for(int j = 0; j < (int)db[i]->collection.size(); j++)
      delete db[i]->collection[j];
    db[i]->collection.clear();
    delete db[i];
  }
  db.clear();
}

int DyMetDB::getdbid(std::string name){
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);
  //auto it = std::find(header.begin(), header.end(), name);
  std::vector<std::string>::iterator it = std::find(header.begin(), header.end(), name);
  if (it == header.end()){
    // name not in vector
    return -1;
  }
  else{
    return std::distance(header.begin(), it);
  }
}

std::vector<int> DyMetDB::find_all_keys_id(std::vector<key_value> dc, std::string key)
{
  std::vector<int> matches; // here we put the db id!!
  for(int i = 0; i < (int)dc.size(); i++){
    if(dc[i].key.compare(key) == 0){
      matches.push_back(dc[i].value);
    }
    else{
      continue;
    }
  }

  return matches;
}

 void removeCharsFromString(std::string &str, char* charsToRemove){
  for (unsigned int i = 0; i < strlen(charsToRemove); ++i){
   str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
  }
}

std::vector<std::string> DyMetDB::parseqline(std::string qline)
{
  std::vector<std::string> r;
  std::vector<std::string> a = strsplit(qline, ';');
  for(int i = 0; i < (int)a.size(); i++){
    std::vector<std::string> b = strsplit(a[i], ' ');
    for(int j = 0; j < (int)b.size(); j++){
      removeCharsFromString(b[j], (char*)":%");
      r.push_back(b[j]);
    }
  }
  return r;
}

std::string purgestring(std::string s, std::string rm)
{
  std::string t = s;
  std::string::size_type i = t.find(rm);
  if (i != std::string::npos)
    t.erase(i, s.length());
  return t;
}

std::vector<std::string> DyMetDB::getqline(std::vector<int> ids)
{
  std::vector<std::string> lines;
  if((int)ids.size() > 0){
    // read the query lines using a random access to the database file
    std::string line;
    std::ifstream fdb(dbfile.c_str());
    for(int i = 0; i < (int)ids.size(); i++){
      fdb.seekg(ids[i]);
      getline(fdb, line);
      lines.push_back(line);
    }
    fdb.close();
  }
  return lines;
}



/* find examples:
 * find(MS: 233.2844 error: 5ppm; tR: 10.25 error: 5%);
 * or
 * find(MS: 233.2844 error: 5ppm;);
 * or
 * find(tR: 10.25 error: 5%);
 * or
 * find(Name: Glucuronic acid);
 */

struct mapres { // map results data structure...
  mapres(double ms_error_, double tr_error_, std::string row_) :
      ms_error(ms_error_),
      tr_error(tr_error_),
      row(row_){}
  double ms_error;
  double tr_error;
  std::string row;

  bool operator<(const mapres& other) const
  {
    if(ms_error < other.ms_error){
      if(tr_error < other.tr_error){
        return true;
      }
      else{
        return false;
      }
    }
    else if(fabs(ms_error - other.ms_error) < 1e-4){
      if(tr_error < other.tr_error){
        return true;
      }
      else{
        return false;
      }
    }
    else{
      return false;
    }
  }
};

std::vector<std::string> DyMetDB::find(std::string qline)
{
  //std::cout << "Databas size: " << (int)db[0]->collection.size() << std::endl;
  //std::cout << ">>>>> Search: <<<<<\n" << qline << std::endl;
  std::vector<std::string> q = parseqline(qline);
  std::vector<int> matches; // here we put the db id!!
  bool refine = false;

  /*
  std::cout << "Parsing out: " << std::endl;
  for(int i = 0; i < (int)q.size(); i++){
    std::cout << q[i] << std::endl;
  }
  std::cout << "---------------" << std::endl;
  */
  // Global variables
  double ms = 0.f;
  double add = 0.f;
  double tr = 0.f;
  double perr = 0.f;
  double vm = 0.f;
  double vd = 0.f;
  double flow = 0.f;
  double init_B = 0.f;
  double final_B = 0.f;
  double tg = 0.f;

  int idName = getdbid("Name");
  int idMS = getdbid("MS");
  int idMSSpec = getdbid("Formula");
  int idLogKw = getdbid("LogKw");
  int idS = getdbid("S");

  for(int i = 0; i < (int)q.size(); i+=2){
    if(q[i].compare("Name") == 0){
      //int id = 0; // header
      //int idName = getdbid("Name");
      for(int j = 0; j < (int)db[idName]->collection.size(); j++){
        if(db[idName]->collection[j]->key.compare(q[i+1]) == 0){
          matches.push_back(db[idName]->collection[j]->value);
        }
        else{
          continue;
        }
      }
      std::vector<std::string> lines = getqline(matches);
      for(int j = 0; j < (int)lines.size(); j++){
        std::vector<std::string> v = strsplit(lines[j], ';');
      }
      refine = true;
    }
    else if(q[i].compare("MS") == 0){
      //int id = 1; //header
      //int idMS = getdbid("MS");
      if(q[i+1].find("m/z") != std::string::npos)
        q[i+1].erase(std::remove(q[i+1].begin(), q[i+1].end(), 'm/z'), q[i+1].end());
      else if(q[i+1].find("n") != std::string::npos)
        q[i+1].erase(std::remove(q[i+1].begin(), q[i+1].end(), 'n'), q[i+1].end());

      //std::cout << "MS converted: " << q[i+1] << std::endl;
      ms = stod_(q[i+1]);
      double ppm = 0.f;
      int sz = 0;
      if(i+5 <= (int)q.size())
        sz = i+5;
      else
        sz = (int)q.size();

      for(int j = 0; j < sz; j++){
        if(q[j].compare("error") == 0){
          ppm = stod_(purgestring(q[j+1], "ppm"));
        }
        else if(q[j].compare("add") == 0){
          add = stod_(q[j+1]);
        }
        else
          continue;
      }

      double mserror = DaltonError(ms, ppm);

      //std::cout << "Search for... " << ms << " " << ppm << " " << mserror << " "<<  add << std::endl;
      //second column is ms
      if(matches.size() == 0 && refine == false){ // search starting from MS
        for(int j = 0; j < (int)db[idMS]->collection.size(); j++){
          //std::cout << stod_(db[idMS]->collection[j]->key) << " " << add << " " << stod_(db[idMS]->collection[j]->key) + add << " " << ms << std::endl;
          if(std::fabs((stod_(db[idMS]->collection[j]->key)+add) - ms) <= mserror){
            //std::cout << stod_(db[idMS]->collection[j]->key) << " " << add << " " << stod_(db[idMS]->collection[j]->key) + add << " " << ms << " " << mserror << std::endl;
            matches.push_back(db[idMS]->collection[j]->value);
          }
          else{
            continue;
          }
        }
      }
      else{ // refine other search by MS
        std::vector<std::string> lines = getqline(matches);
        for(int j = 0; j < (int)lines.size(); j++){
          std::vector<std::string> v = strsplit(lines[j], ';');
          if(std::fabs(stod_(v[idMS]) - ms) <= mserror){
            continue;
          }
          else{
            matches.erase(matches.begin()+j);
          }
        }
      }
      refine = true;
    }
    /*
    else if(q[i].compare("MSSpectra") == 0){
      //Example MSSpectre: 49.800:39604.0, 56.700:19802.0, 98.200:29703.0, 128.800:1514853.0, 147.100:57420849.5;
      std::vector<std::string> ip = strsplit(q[i+1], ',');
      if(matches.size() == 0 && refine == false){ // search starting from tR
        for(int k = 0; k < ip.size(); k++){
          std::cout << ip[i] << std::endl;
        }

        for(int j = 0; j < (int)db[idMSSpec]->collection.size(); j++){
          continue;
          //strsplit(lines);
        }
        refine = true;
      }
      else{

      }
    }
    else if(q[i].compare("MSMSSpectre") == 0){

    }*/
    else if(q[i].compare("tR") == 0){
      //int idLogKw = getdbid("LogKw");
      //int idS = getdbid("S");
      tr = stod_(q[i+1]);

      if(i+1+15 > (int)q.size()){
        std::cout << "Error in method definition!" << std::endl;
      }

      for(int j = i+1; j < i+15; j++){
        if(q[j].compare("error") == 0){
          perr = stod_(purgestring(q[j+1], "%"));
        }
        else if(q[j].compare("init") == 0){
          init_B = stod_(q[j+1]);
          if(init_B > 1)
            init_B /= 100.f;
        }
        else if(q[j].compare("final") == 0){
          final_B = stod_(q[j+1]);
          if(final_B > 1)
            final_B /= 100.f;
        }
        else if(q[j].compare("tg") == 0){
          tg = stod_(q[j+1]);
        }
        else if(q[j].compare("flow") == 0){
          //std::cout << q[j] << " " << q[j+1] << std::endl;
          flow = stod_(q[j+1]);
        }
        else if(q[j].compare("vm") == 0){
          vm = stod_(q[j+1]);
        }
        else if(q[j].compare("vd") == 0){
          vd = stod_(q[j+1]);
        }
        else{
          continue;
        }
      }

      //std::cout << "tr: " << tr << " err: " << perr << " vm: " << vm << " vd: " << vd << " flow: " << flow << " init: " << init_B << " final: " << final_B << " tg: " << tg << std::endl;
      if(matches.size() == 0 && refine == false){ // search starting from tR
        for(int j = 0; j < (int)db[idLogKw]->collection.size(); j++){
          if(tr > -1 && perr > -1){
            double logkw = stod_(db[idLogKw]->collection[j]->key);
            double s = stod_(db[idS]->collection[j]->key);
            double tr_pred = rtpred(logkw, s, vm, vd, flow, init_B, final_B, tg);
            if(std::fabs((tr - tr_pred)/tr)*100.f <= perr){
              matches.push_back(db[idLogKw]->collection[j]->value);
            }
            else{
              continue;
            }
          }
          else{
            // no retention error comparisson needed
            matches.push_back(db[idLogKw]->collection[j]->value);
          }
        }
      }
      else{ //refine search
        std::vector<std::string> lines = getqline(matches);
        int j = 0;
        while(j != (int)lines.size()){
          if(tr > -1 && perr > -1){
            std::vector<std::string> v = strsplit(lines[j], ';');
            double logkw = stod_(v[idLogKw]);
            double s = stod_(v[idS]);
            double tr_pred = rtpred(logkw, s, vm, vd, flow, init_B, final_B, tg);
            if(std::fabs((tr - tr_pred)/tr)*100.f <= perr){
              j++;
              continue;
            }
            else{
              matches.erase(matches.begin()+j);
              lines.erase(lines.begin()+j);
              j = 0;
            }
          }
          else{
            // no retention error comparisson needed
            j++;
          }
        }
      }
      refine = true;
    }
    else{
      continue;
    }
  }

  // Give output
  // Name;mass_identified;error_ms;tr;error_tr %;
  // rank output from the small errors in retention time and ppm
  std::vector<std::string> res;
  if(matches.size() == 0){
    //std::cout << "No object found!" << std::endl;
    return res;
  }
  else{
    std::vector<std::string> lines = getqline(matches);
    std::vector<mapres> tmprow;
    for(int i = 0; i < (int)lines.size(); i++){
      std::vector<std::string> v = strsplit(lines[i], ';');
      //std::string row;
      std::stringstream row;
      double ms_error = 0.f;
      double tr_error = 0.f;
      //std::cout << "ms calculation & rt calculation" << std::endl;
      //std::cout << tr << " " << vm << " " << vd << std::endl;
      if(init_B > 0 && final_B > 0 && tg > 0 && vm > 0 && vd > 0){ // rt calculation
        double logkw = stod_(v[idLogKw]);
        double s = stod_(v[idS]);
        //std::cout << logkw << " " << s << std::endl;
        double tr_pred = rtpred(logkw, s, vm, vd, flow, init_B, final_B, tg);
        //std::cout << tr_pred << std::endl;
        if(ms > 0){
          // ppm error = (mass_observed - mass_calcuated) / mass_calcualted * 1e6
          double mass_calcuated = stod_(v[idMS])+add;
          //std::cout << ms << " " << v[idMS] << " " << add << " " << mass_calcuated << std::endl;
          ms_error = ((ms - mass_calcuated)/mass_calcuated)*1e6;
        }

        if(tr > 0){
          tr_error = (fabs(tr_pred-tr)/tr)*100;
        }

        row << header[idName] << ": "<< v[idName] << ";"; // name
        row << header[idMS] << ": " << NumberToString(v[idMS]) << ";"; // MS
        row << "MS ERROR: " << NumberToString(pround(ms_error, 3)) << ";"; // MSERROR
        row << "tR: " << NumberToString(pround(tr_pred, 3)) << ";"; // Retention time predicted
        row << "%tR error: " << NumberToString(pround(tr_error, 3)) << ";"; // Retention time error

        // add other info!
        for(int j = 0; j < header.size()-1; j++){
          if(j != idName && j != idMS && j != idLogKw && j != idS){
            row << header[j] << ": " << v[j] << ";";
          }
          else{
            continue;
          }
        }

        int j = header.size()-1;
        if(j != idName && j != idMS && j != idLogKw && j != idS){
          row << header.back() << ": "+v.back();
        }
      }
      else{
        row << header[idName] << ": " << v[idName] << ";"; // name
        row << header[idMS] << ": " << NumberToString(v[idMS]) << ";"; // MS

        double mass_calcuated = stod_(v[idMS])+add;
        ms_error = ((ms - mass_calcuated)/mass_calcuated)*1e6;

        row << "MS ERROR: " << NumberToString(pround(ms_error, 3)) << ";"; // MSERROR

        // add other info!
        for(int j = 0; j < header.size()-1; j++){
          if(j != idName && j != idMS && j != idLogKw && j != idS){
            row << header[j] << ": " << v[j] << ";";
          }
          else{
            continue;
          }
        }

        int j = header.size()-1;
        if(j != idName && j != idMS && j != idLogKw && j != idS){
          row << header.back() << ": " << v.back();
        }
      }
      tmprow.push_back(mapres(ms_error, tr_error, row.str()));
    }
    std::sort(tmprow.begin(), tmprow.end());
    for(int i = 0; i < tmprow.size(); i++)
      res.push_back(tmprow[i].row);
    //res.push_back(row);
    return res;
  }
}

void DyMetDB::setRTLinearAligner(std::string rttunfile, std::string qline)
{
  rtslope = 1.f;
  rtintercept = 0.f;
  std::vector<double> x;
  std::vector<double> y;
  std::string line;
  std::ifstream frttun(rttunfile.c_str());
  if(frttun.is_open()){
    while(getline(frttun, line)){
      std::vector<std::string> v = strsplit(line, ';');
      if(v.size() == 2){
        std::stringstream ss;
        ss << "Name " << v[0] << ";" << qline;
        /* qline is of type: Name: HMDB00253; tR: -1 error: -1 init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.3099 vd: 0.375 */
        std::vector<std::string> res = find(ss.str());
        if(res.size() == 1){
          //Name: HMDB01547;MS: 346.2144094;MS ERROR: nan;tR: -1%tR error: 1.12;FORMULA: C21H30O4;STATE: EXPERIMENTAL
          std::vector<std::string> a = strsplit(res[0], ';');
          std::vector<std::string> b = strsplit(a[3], ':');
          x.push_back(stod_(v[1]));
          y.push_back(stod_(b[1]));
        }
        else{
          continue;
        }
      }
    }

    std::vector<std::string> q = parseqline(qline);

    /*Calculate rtslope and rtintercept by linear regression.
     * y = ax + b
     *  b = [Sum y_i(x_i-X)] / [Sum x_i(x_i-X)] were X is the x average;
     *  a = Y-Xb were Y is the y average;
     */

    // Calculating averages
    double x_avg = 0.f; double y_avg = 0.f;
    for(int i = 0; i < x.size(); i++){
      x_avg += x[i];
      y_avg += y[i];
    }
    x_avg /= (double)x.size();
    y_avg /= (double)y.size();

    double n = 0.f; double d = 0.f;
    for(int i = 0; i < x.size(); i++){
      n += y[i]*(x[i]-x_avg);
      d += x[i]*(x[i]-x_avg);
    }
    // Calculation tR corrective parameters.
    rtslope = n/d;
    rtintercept = y_avg - x_avg*rtslope;
    frttun.close();
  }
}

double DyMetDB::rtpred(double logkw, double s,
  double vm, double vd, double flow, double init_B, double final_B, double tg)
{
  double t0 = vm/flow;
  double td = vd/flow;
  double DeltaFi = final_B - init_B;
  double b = (t0 * DeltaFi * s) / tg;
  double k0 = pow(10, (logkw - s*(init_B)));
  double trpred = ((t0/b) * log10(2.3*k0*b))+ t0 + td;
  /* y = ax + b where in this case rtslope
  * and rtintercept are estimated using already identified
  * compounds in a run
  * N.B.: if no setRTTuning is run, rtslope = 1 and rtintercept = 0
  */
  return (trpred - rtintercept)/rtslope;
}

double DyMetDB::DaltonError(double mass, double ppm)
{
  // error(Da) = (theoretical mass) / ((1/errorPPM)*1,000,000)
  return mass/((1/ppm) * 1000000);
}


double DyMetDB::pround(double x, int precision)
{
  if (x == 0.)
    return x;
  int ex = floor(log10(fabs(x))) - precision + 1;
  double div = pow(10, ex);
  return floor(x / div + 0.5) * div;
}

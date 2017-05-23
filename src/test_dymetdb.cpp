#include <iostream>
#include <string>
#include "dymetdb.h"

void PrintRes(std::vector<std::string> r)
{
  std::cout << "___________FOUND___________ " << std::endl;
  for(int i = 0; i < (int)r.size(); i++)
    std::cout << r[i] << std::endl;
  std::cout << "___________________________ " << std::endl;
}

int main(int argc, char **argv)
{
  std::vector<std::string> r;

  DyMetDB *db = new DyMetDB;
  // test init database
  db->init(argv[1]);

  /*db->init("../../src/testdymetdb.txt");*/
  /*std::cout << "Find by name" << std::endl;
  r = db->find("Name: 16a-Hydroxytestosterone");
  PrintRes(r);
  r.clear();
  std::cout << "Find by MS" << std::endl;
  r = db->find("MS: 305.2100 error: 5ppm add: 1.0072764649920167");
  PrintRes(r);
  r.clear();*/
  std::cout << "Find by MS MSSpectra and tR" << std::endl;
  //MSSpectra: 286.208:0,288.208:100,289.212:20.54988,289.213:0.07619,289.215:0.32204,290.213:0.411,290.215:2.00036,290.216:0.01566,290.218:0.06618,291.216:0.08446,291.218:0.1226,291.219:0.00285,291.221:0.00644,292.219:0.00822,292.222:0.0053,294.222:0;
  r = db->find("MS: 305.2100 error: 5ppm add: 1.0072764649920167; MSSpectra: 286.208:0,288.208:100,289.212:20.54988,289.213:0.07619,289.215:0.32204,290.213:0.411,290.215:2.00036,290.216:0.01566,290.218:0.06618,291.216:0.08446,291.218:0.1226,291.219:0.00285,291.221:0.00644,292.219:0.00822,292.222:0.0053,294.222:0; tR: 7.57 error: 2% init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.2907 vd: 0.375");
  /* r = db->find("Name: HMDB01547; tR: -1 error: -1 init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.3099 vd: 0.375"); */
  /*r = db->find("MS: 435.96 error: 500ppm");*/
  PrintRes(r);
  r.clear();
  delete db;
  return 0;
}

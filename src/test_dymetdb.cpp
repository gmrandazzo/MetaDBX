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

  /*db->init("../../src/testdymetdb.txt");
  r = db->find("Name: molecule3");
  PrintRes(r);
  r.clear();
  r = db->find("Name: molecule1");
  PrintRes(r);
  r.clear();
  r = db->find("Name: molecule4");
  PrintRes(r);
  r.clear();*/
  //r = db->find("MS: 316.240 error: 50ppm add: -17.00281461955999");
  r = db->find("MS: 347.2219 error: 25ppm add: 1.0079; tR: 9.05 error: 5% init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.3099 vd: 0.375");
  /* r = db->find("Name: HMDB01547; tR: -1 error: -1 init: 5 final: 95 tg: 14 flow: 0.3 vm: 0.3099 vd: 0.375"); */
  /*r = db->find("MS: 435.96 error: 500ppm");*/
  PrintRes(r);
  r.clear();
  delete db;
  return 0;
}

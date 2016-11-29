# MetaDBX

Metabolite database eXtraction is a software complement to help the automatic annotation. 
It is composed by two modules:
- MetaSDF-X which allows to extract/merge SDF structures from HMDB and explot the metabocard informations
- DynMetID which allows to identify metabolites by retention time recalculated according a specific chromatographic gradient condition.
  For that purpose it require a database of linear solvent strength (LSS) parameters which could be predicted by QSRR or experimentally determine.


![ScreenShot](https://github.com/gmrandazzo/MetaDBX/blob/master/screenshot.png)


License
============

MetaDBX is distributed under LGPLv3 license.

Copyright (C) 2016 Giuseppe Marco Randazzo gmrandazzo@gmail.com

To know more in details how the licens work please read the file "LICENSE" or
go to "http://www.gnu.org/licenses/lgpl-3.0.html"


Dependencies
============
- a c++ compiler
- cmake >= 2.8.9
- Qt5



Install
=======

To install for all users on Unix/Linux/OSX/Windows:


 cmake .. -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.7.0/ 
 make


You need help? Support?
=======================

Write to: gmrandazzo@gmail.com







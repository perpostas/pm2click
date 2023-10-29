# pm2click
A command line Linux utility developed for parsing packet core XML statistical files.  

The goal of pm2click is to parse XML statistical file and transform XML counters data to TSV format.  
The utility uses libxml2 library SAX parser.


### License

This code is released under the MIT License, see the Copyright file.


### Build instructions

pm2click can be built with CMake.

1) Download the source files archive
2) tar xf pm2click-x.x.x.tar.gz
3) cd pm2click-x.x.x
4) cmake -S . -B ./build


### Dependencies

pm2click requires libxml2 library (MIT licenced) to be installed in your linux system.


### Contributing

The current version of the code can be found GitHub 
at <https://github.com/perpostas/pm2click.git>.  
The best way to get involved is by creating issues and merge requests on GitHub.


### Authors
Included in source files comments

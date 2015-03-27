# Step-by-step building morda in Windows using MinGW and MSYS #

  1. **MSYS**
    * Download **mingw installer** from [here](http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/mingw-get-inst-20120426/) and run it. Follow installation instructions bearing in mind the following:
      * When it asks about which components to install, select the following:
        * C Compiler
        * C++ Compiler
        * MSYS Basic System
        * MinGW Developer ToolKit
      * After installation add path to MinGW and MSYS to environment variable PATH. Usually, those paths are `C:\MinGW\bin` and `C:\MinGW\msys\1.0\bin`
      * Make a link to `C:\MinGW\msys\1.0\msys.bat` and place it to Desktop in order to be able to conveniently start MSYS shell.
  1. **MinGW-w64**
    * install mingw w64 and replace C:\MinGW\mingw32 with mingw32 from w64 installation.
    * edit fstab to mount mingw32 to /mingw
  1. **SlikSVN**
    * Download **SlikSVN** from [here](http://www.sliksvn.com/en/download), choose the package depending on your system, 32 or 64 bit.
    * Run the installer and follow instructions. Typical installation should be enough.
    * Open MSYS console (Start menu/MinGW/MinGW Shell). Note, that disks **C:**, **D:** etc. are mounted as **/c** and **/d** and so on. In the home folder create file called **.bashrc** with the following contents:
      * `export PATH=$PATH:/c/Program\ Files/SlikSvn/bin`
  1. **ting**
    * Check out **ting** sources to some directory, say **/d/ting**:
      * `svn co http://ting.googlecode.com/svn/trunk/ /d/ting`
    * `cd /d/ting`
    * `make`
    * `make install`
  1. **stob**
    * Check out **stob** sources to some directory, say **/d/stob**:
      * `svn co http://stob.googlecode.com/svn/trunk/ /d/stob`
    * `cd /d/stob`
    * `make`
    * `make install`
  1. **GLEW**
    * Download **GLEW** sources from [here](http://sourceforge.net/projects/glew/files/glew/1.9.0/glew-1.9.0.zip/download) and unzip them to some directory, say **/d**. Then the sources will reside in **/d/glew-1.9.0**.
    * `cd /d/glew-1.9.0`
    * `make`
    * `make install`
  1. **zlib**
    * Download **zlib** sources from [here](http://zlib.net/zlib-1.2.8.tar.gz) and unpack them to some directory, say **/d**, then the sources will reside in **/d/zlib-1.2.8**.
    * `cd /d/zlib-1.2.8`
    * `make -f win32/Makefile.gcc`
    * `make -f win32/Makefile.gcc install INCLUDE_PATH=/usr/local/include LIBRARY_PATH=/usr/local/lib BINARY_PATH=/usr/local/lib`
  1. **libpng**
    * Download **libpng-1.6.0** sources from [here](http://sourceforge.net/projects/libpng/files/libpng16/1.6.0/lpng160.zip/download) and unzip sources to some directory, say **/d**, then sources will reside in **/d/lpng160**.
    * `cd /d/lpng160`
    * `cp scripts/makefile.msys makefile`
    * `make`
    * `make install`
  1. **libjpeg**
    * Download **libjpeg** sources from [here](http://ijg.org/files/jpegsrc.v9.tar.gz) and untargz them to some directory, say **/d**, then sources will reside in **/d/jpeg-9**. Note, that it is important to download tar.gz package, NOT .zip one.
    * `cd /d/jpeg-9`
    * `./configure`
    * `make`
    * `make install`
  1. **libfreetype**
    * Download **libfreetype** sources from [here](http://sourceforge.net/projects/freetype/files/freetype2/2.4.11/freetype-2.4.11.tar.gz/download) and untargz them to some directory, say **/d**, then sources will reside in **/d/freetype-2.4.11**.
    * `cd /d/freetype-2.4.11`
    * `./configure`
    * `make`
    * if getting error about off64\_t and off\_t, then it is a bug in MinGW, see http://sourceforge.net/p/mingw/bugs/2024/ how to resolve.
    * `make install`
  1. **morda**
    * Check out **morda** sources to some directory, say **/d/morda**:
      * `svn co http://morda.googlecode.com/svn/trunk/ /d/morda`
      * `cd /d/morda`
      * `make debug=true`
      * `make test`
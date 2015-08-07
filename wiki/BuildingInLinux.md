# How to build and install morda in Linux #

## Step by step instructions ##

  1. Install necessary libraries
```
sudo apt-get install libpng12-dev libglew-dev libjpeg8-dev libfreetype6-dev doxygen
```
  1. Check out **ting** sources and install:
```
svn co http://ting.googlecode.com/svn/trunk/ ting
cd ting
make deb
sudo dpkg -i ../libting*
```
  1. Check out **stob** sources and install:
```
svn co http://stob.googlecode.com/svn/trunk/ stob
cd stob
make deb
sudo dpkg -i ../libstob*
```
  1. Check out **morda** sources, install it and run test app:
```
svn co http://morda.googlecode.com/svn/trunk/ morda
cd morda
make deb
sudo dpkg -i ../libmorda*
make test
```
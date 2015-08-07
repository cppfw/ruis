# How to build and install morda in Mac OS X #

## Step by step instructions ##

  1. Install command line tools. This is needed to make compilers look for headers in /usr/local/include
```
xcode-select --install
```
  1. Make sure you have clang 5.1 or higher installed:
```
clang --version
```
  1. Install **homebrew** if it is not installed yet. Refer to [homebrew homepage](http://brew.sh) in case of problems.
```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
  1. run brew doctor and fix reported problems if possible
```
sudo brew doctor
```
  1. Install necessary libraries
```
brew install lzlib libpng jpeg glew freetype
```
  1. Check out **ting** sources and install:
```
svn co http://ting.googlecode.com/svn/trunk/ ting
cd ting
make
sudo make install
```
  1. Check out **stob** sources and install:
```
svn co http://stob.googlecode.com/svn/trunk/ stob
cd stob
make
sudo make install
```
  1. Check out **morda** sources, install it and run test app:
```
svn co http://morda.googlecode.com/svn/trunk/ morda
cd morda
make
sudo make install
make test
```
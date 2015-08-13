# How to build morda in Linux #

## Step by step instructions ##

  * Install necessary libraries
```
sudo apt-get install libpng12-dev libglew-dev libjpeg8-dev libfreetype6-dev doxygen
```
  * Add
```
deb https://repo.fury.io/igagis/ /
```
  to APT sources and install **ting** and **stob**

```
  sudo apt-get update
  sudo apt-get install libting-dev libstob-dev
```
  * Check out **morda** sources, install it and run test app:
```
git clone https://github.com/igagis/morda.git
cd morda
make
make test
```

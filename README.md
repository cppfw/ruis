# morda [![releases](https://img.shields.io/github/tag/igagis/morda.svg)](https://github.com/igagis/morda/releases) [![Build Status](https://travis-ci.org/igagis/morda.svg?branch=master)](https://travis-ci.org/igagis/morda) [![Build status](https://ci.appveyor.com/api/projects/status/vnce10b7pqgfvfug/branch/master?svg=true)](https://ci.appveyor.com/project/igagis/morda/branch/master)


OpenGL based GUI library in C++

[![Screenshot](wiki/images/screenshot2_thumbnail.png)](https://raw.githubusercontent.com/igagis/morda/master/wiki/images/screenshot2.png)

**morda** is a cross-platform OpenGL based intrusive GUI framework dedicated to games and multimedia applications. Its design was inspired by GUI system from Android and many concepts are borrowed from there. Application using **morda** will have only one native window created for it and OpenGL initialized.

## Why intrusive?
Desicion in favor of intrusive approach was made in order to transparently support non-traditional application lifetime concepts like it is on Android and iOS.

## Why OpenGL?
The intention was to create a truly cross-platform GUI framework, i.e. there should be no any modifications needed to application code in order to build it for different platforms. Only OpenGL out of modern graphics libraries is cross-platofrm and exists on all major platforms.

OpenGL versions supported:
  * OpenGL 2.0 and higher
  * OpenGL ES 2.0 and higher

## Supported platforms
  * Linux
  * Windows
  * MacOS X
  * Android 2.3+ **planned** (draft implementation exists, currently broken)
  * iOS **planned** (draft implementation exists)

# Useful info
  link:wiki/Contribution.adoc[Contributing instructions]

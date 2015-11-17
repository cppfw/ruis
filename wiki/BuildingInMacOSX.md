# How to build morda in Mac OS X #

## Step by step instructions ##

- Install command line tools. This is needed to make compilers look for headers in /usr/local/include

	```
	xcode-select --install
	```

- Make sure you have clang 5.1 or higher installed:

	```
	clang --version
	```

- Install **homebrew** if it is not installed yet. Refer to [homebrew homepage](http://brew.sh) in case of problems.

	```
	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
	```
	
- run brew doctor and fix reported problems if possible
	
	```
	brew doctor
	```

- Add the homebrew tap (needs to be done once):
	
	```
	brew tap igagis/tap
	brew update
	```

- Install necessary libraries

	```
	brew install lzlib libpng jpeg glew freetype libstob libaika libnitki libkolme
	```

- Check out **morda** sources, install it and run test app:
	
	```
	git clone git@github.com:igagis/morda.git
	cd morda
	make
	make test
	```

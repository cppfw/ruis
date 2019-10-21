# How to build morda in Mac OS X #

## Step by step instructions ##

- Install command line tools. This is needed to make compilers look for headers in /usr/local/include

	```
	xcode-select --install
	```

- Install [homebrew](http://brew.sh) if not installed yet.

- Add the homebrew tap (needs to be done once):
	
	```
	brew tap igagis/tap
	```

- Update homebrew formulaes:

	````
	brew update
	````

- Install necessary libraries

	```
	brew install lzlib libpng jpeg glew freetype lib libaika libnitki libkolme libsvgren
	```

- Check out **morda** sources, build it and run test app:
	
	```
	git clone git@github.com:igagis/morda.git
	cd morda
	make debug=true
	make test
	```

# How to build morda in Linux #

## Step by step instructions ##
- Add the repo to your sources (needs to be done once):
	
	```
	echo "deb https://dl.bintray.com/igagis/deb /" | sudo tee /etc/apt/sources.list.d/igagis.list > /dev/null
	```

- Update apt packages
	
	```
	sudo apt-get update
	```

- Install necessary libraries
	
	```
	sudo apt-get install libpng12-dev libglew-dev libjpeg8-dev libfreetype6-dev doxygen libstob-dev libnitki-dev libaika-dev libkolme-dev
	```

- Check out **morda** sources, build it and run test app:
	```
	git clone https://github.com/igagis/morda.git
	cd morda
	make
	make test
	```

# libusbstorage
USB Storage utility 

This library become useful if your application require to monitor external usbstorage and to mount at a specifiedmount point.

# Build instructions

System Dependencies:

```
sudo apt-get install libudev-dev
```

Then buildand install the following general purpose library:

**FEDLibrary** - *http://sourceforge.net/projects/fedlibrary/*
```
git clone git://git.code.sf.net/p/fedlibrary/git fedlibrary
cd fedlibrary 
git checkout v3.2.0
mkdir build 
cd build
cmake ../ -DFORCE_PIC=ON 
make 
make install 
ldconfig 
cd ../../ 
```

**libusebstorage** - *here you are*
```
git clone https://github.com/fe-dagostino/libusbstorage.git libusbstorage
cd libusbstorage
mkdir build 
cd build
cmake ../ 
make 
```

In order to install usbmond please follow instruction in file ./install/linux/README .




# radon
Implementation of Radon algorithm in C++ with Qt interface

How to run on macOS:
1. Install opencv and qt
```
brew install qt5 opencv3
echo 'export PATH="/usr/local/opt/opencv@3/bin:$PATH"' >> ~/.bash_profile
export LDFLAGS="-L/usr/local/opt/opencv@3/lib"
export CPPFLAGS="-I/usr/local/opt/opencv@3/include"
export PKG_CONFIG_PATH="/usr/local/opt/opencv@3/lib/pkgconfig"
```
2. Compile the project:
```
qmake && make
```
3. Run compiled binary
```
radon.app/Contents/MacOS/radon
```

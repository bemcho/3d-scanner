# 3d-scanner
You need opencv built with-v4l in order to read from avi files 

sudo apt-get install libv4l-dev 
cmake -DWITH_LIBV4L=ON .. (this is important)
to install needed libs 

cd 3d-scanner/build

cmake ..

make

cd ../bin

./3DScanner yourmovie.avi/webm/mp4 etc.

If started without command line args or passed filename is not fouind,
the app will try to use the first camera it finds starting from: (0 for default eg. embedded camera).


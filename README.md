# 3d-scanner
You need opencv built with-v4l in order to read from avi files 

sudo apt-get install libv4l-dev 
cmake -DWITH_LIBV4L=ON .. (this is important)
to install needed libs 

cd 3d-scanner/build
cmake ..
make

./3DScanner yourmovie.avi/webm/mp4 etc.

If started without command line args will try to start the first camera it founds starting from: (0 for default eg. embedded camera).


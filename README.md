# Andama Remote Desktop
Andama is an open source remote desktop software, with client side encryption and privacy in mind

### How to compile con Ubuntu 16.04 x86_64

#### 1) Install the required dependeces
    apt-get update
    apt-get install qt5-default libxtst-dev build-essential git -y

#### 2) Checkout the code
    git clone https://github.com/Dark-Vex/Andama-Remote-Desktop.git

#### 3) Compile AndamaProxy (Server)
    cd Andama-Remote-Desktop/src/AndamaProxy
    qmake Andama.pro
    make clean
    make

#### 4) Compile Andama (Client)
If you whish to use your own server before start compiling you should edit the SERVER string inside clientserver.cpp

    cd Andama-Remote-Desktop/src/AndamaProxy
    qmake Andama.pro
    make clean
    make

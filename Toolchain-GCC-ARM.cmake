
# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)
# specify the cross compiler
SET(CMAKE_C_COMPILER /usr/local/AvtecGcc-1.0/ARM/armv6-eabihf--glibc--stable-2018.02-2/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /usr/local/AvtecGcc-1.0/ARM/armv6-eabihf--glibc--stable-2018.02-2/bin/arm-linux-g++)
SET(CMAKE_CC_COMPILER /usr/local/AvtecGcc-1.0/ARM/armv6-eabihf--glibc--stable-2018.02-2/bin/arm-linux-gcc)
# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH /usr/local/AvtecGcc-1.0/ARM/armv6-eabihf--glibc--stable-2018.02-2/bin)

# specify the cross compiler
#SET(CMAKE_C_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-g++)
#SET(CMAKE_CC_COMPILER /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
# where is the target environment 
#SET(CMAKE_FIND_ROOT_PATH /home/yackey/RepoBase/BuildrootBase/buildroot-2017.11.1/output/host/bin)



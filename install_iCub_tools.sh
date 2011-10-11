#!/bin/bash
#
# icub_install - iCub simulator installation script - version 5.3 - October 11
# Copyright 2011 Giuseppe Cuccu, http://www.idsia.ch/~giuse all rights reserved
# Licensed under CC BY-SA 3.0 http://creativecommons.org/licenses/by-sa/3.0/
#
# Thanks to Kail Frank, Alessandro Andreani, and all the IDSIA IM-CLEVER team
# Refsite: http://inside.idsia.ch/pmwiki.php?n=IDSIA.Icub


# Available software versions:
# - yarp 2.3.{0-8}    http://sourceforge.net/projects/yarp0/files/yarp2/
# - icub 1.1.{0,3-5}  http://eris.liralab.it/iCub/downloads/src/
#
# Tested combinations:
# Ubuntu 10.04
##  yarp /  icub /   ubuntu /// comp / ipopt / jyarp / leosim / vskin
#  2.3.3 / 1.1.0 / 10.04_64 ->   yes /  yes  /  yes  /   yes  /  yes
#  2.3.3 / 1.1.0 / 10.04_32 ->   yes /  yes  /  yes  /   yes  /  yes
#  2.3.4 / 1.1.0 / 10.04_64 ->   yes /  yes  /  yes  /   yes  /  yes
#  2.3.4 / 1.1.0 / 10.04_32 ->   yes /  yes  /  yes  /   yes  /  yes
# Ubuntu 11.04
##  yarp /  icub /   ubuntu /// comp / ipopt / jyarp / leosim / vskin
#  2.3.3 / 1.1.0 / 11.04_64 ->  NOPE (bug: ControlBoardWrapper2) / / / /
#  2.3.4 / 1.1.0 / 11.04_64 ->  NOPE (bug: ControlBoardWrapper2) / / / /
#  2.3.4 / 1.1.3 / 11.04_64 ->   yes /  no (activable)  /  yes  /   yes  / not tryed
#  2.3.8 / 1.1.5 / 11.04_64 ->   yes /   no  /  yes   / new version / yes
#
# Current suggested combination: 2.3.8 / 1.1.5 / 11.04_64 / newleosim / newvskin
#

############################################

#CONFIGURATION

# Installation directory
DIR=${HOME}/icub_sw

# yarp and icub versions
YARP_V="2.3.8"   # Last supported: 2.3.8
ICUB_V="1.1.5"   # Last supported: 1.1.5

# additional options
INTERFS=true     # please use lowercase true or false
#LEOSIM="/home/giuse/iCubSimulation_LEO.tar.bz2" # point to OLD Leo's simulator
LEOSIM="${HOME}/iCubSimulation_LEO_new.tar.bz2" # point to NEW Leo's simulator

# Debug printout and pauses
debug=false

# To reorder...
  GET_XTRA_SW=true
  XTRA_SW="control.sh iCubSimulation_LEO_new.tar.bz2 vskin.tar.bz2"
  INTDIR="${DIR}/yarp_interfaces"
  BASHRC="${HOME}/.bashrc"
  CONTROLSCRIPT="${HOME}/control.sh"
  VSKIN="${HOME}/vskin.tar.bz2"
  VSKIN_DIR=${DIR}/vskin
  LIBSOLID_V="3.5.6"

  APT="apt-get"
  # Leave the following deactivated until they solve the ipopt_addlib.txt bug / MUMPS is missing!
  IPOPT="FALSE"    # please use uppercase "TRUE" or "FALSE"
############################################

if ${debug}; then set -x; fi

download_from_giuse(){
  for f in ${XTRA_SW}; do
    wget -P $HOME http://www.idsia.ch/~giuse/utilities/${f};
  done
  chmod a+x control.sh
}


gain_root_access(){

  echo " Obtaining root access for system-wide operations"

  echo "Please enter your password to install dependencies:"
  sudo ls > /dev/null 2>&1

  echo " Root access obtained"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


download_and_extract_software() {

  echo " Downloading and extracting software"

  mkdir -p ${DIR}
  cd ${DIR}
  wget http://downloads.sourceforge.net/project/yarp0/yarp2/yarp-${YARP_V}/yarp-${YARP_V}.tar.gz &
  wget http://eris.liralab.it/iCub/downloads/src/iCub-src-${ICUB_V}.tar.gz
  rm -rf ${DIR}/yarp-${YARP_V}
  rm -rf ${DIR}/iCub
  rm -rf ${DIR}/yarp_interfaces 
  tar xzf yarp-${YARP_V}.tar.gz &
  tar xzf iCub-src-${ICUB_V}.tar.gz

  echo " Software extracted"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


copy_over_LEO_sim(){

  echo " Copying over LEO's simulator..."

  cd ${ICUB_ROOT}/src/simulators
  tar cjf iCubSimulation{.tar.bz2,}
  rm -rf iCubSimulation
  tar xjf ${LEOSIM} -C .
  cd ${ICUB_ROOT}

  echo " LEO's sim ready"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


activate_repositories(){

  echo " Activating and updating repositories..."

  #UBUNTU_V=$(lsb_release -sc)
  #sudo sh -c "echo \"
  #deb http://archive.ubuntu.com/ubuntu ${UBUNTU_V} universe
  #deb-src http://archive.ubuntu.com/ubuntu ${UBUNTU_V} universe
  #\" >> /etc/apt/sources.list"
  # alternative: sudo add-apt-repository "http://archive.ubuntu.com/ubuntu ${UBUNTU_V} universe"
  # sudo sed -i -e "s/# deb http/deb http/g" /etc/apt/sources.list
  sudo add-apt-repository "http://archive.ubuntu.com/ubuntu universe"
#  sudo add-apt-repository "http://archive.ubuntu.com/ubuntu multiverse"
  sudo ${APT} update

  echo " Repositories ready"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


install_dependancies(){

  echo " Installing dependencies..."

  
  sudo ${APT} update && sudo ${APT} -y install cmake cmake-curses-gui gsl-bin g++ libncurses5-dev \
  libace-dev libgtkmm-2.4-dev libglademm-2.4-dev libqt3-mt-dev libgsl0-dev libcv-dev libhighgui-dev \
  libcvaux-dev subversion libode-dev libsdl-image1.2-dev libcv-dev libcvaux-dev glutg3-dev \
  && sudo ldconfig

  echo " Dependancies installed"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


set_environmental_variables(){

  echo " Setting environmental variables..."

  export YARP_DIR="${DIR}/yarp-${YARP_V}"
  export ICUB_ROOT="${DIR}/iCub/main"
  export IPOPT_DIR=/usr  # yeah, it's THIS ugly

  echo " Environmental variables set"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


add_environmental_variables_to_bashrc(){

  echo " Adding environmental variables to ${BASHRC}..."

  if $(grep ICUB_ROOT ${BASHRC}); then
    sudo sed -i -e "s/ICUB_ROOT=/OLD_ICUB_ROOT=/g" ${BASHRC}
    sudo sed -i -e "s/YARP_DIR=/OLD_YARP_DIR=/g" ${BASHRC}
  fi
  echo -e "export ICUB_ROOT=${ICUB_ROOT}\nexport YARP_DIR=${YARP_DIR}\n" >> ${BASHRC}

  echo " Environmental variables added to ${BASHRC}"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


compile_yarp(){

  echo " Compiling YARP..."

  cd ${YARP_DIR}
  cmake -D CREATE_GUIS:BOOL=TRUE -D CREATE_LIB_MATH:BOOL=TRUE \
        -D CREATE_SHARED_LIBRARY:BOOL=TRUE . ##### shared library needed only for jyarp
  make

  echo " YARP compiled"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


install_yarp(){

  echo " Installing YARP system-wide..."

  cd ${YARP_DIR}
  sudo make install

  echo " YARP installed"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


install_interfaces_dependancies(){

  echo " Installing interfaces dependencies..."

  sudo sh -c "echo sun-java6-jre shared/accepted-sun-dlj-v1-1 select true | debconf-set-selections"
  sudo add-apt-repository "http://archive.canonical.com/ubuntu partner" && sudo ${APT} update && \
  sudo ${APT} -y install sun-java6-jre sun-java6-bin sun-java6-jdk sun-java6-plugin sun-java6-fonts \
                         swig python-dev && \
  sudo update-java-alternatives --set java-6-sun

  echo " Interfaces dependancies installed"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


build_yarp_interfaces(){

  echo " Building YARP interfaces..."

  SWIG_DIR=${YARP_DIR}/example/swig
  cd ${SWIG_DIR}

  # Compile Java and Python interfaces for YARP
  cmake -D CREATE_JAVA:BOOL=TRUE          -D YARP_AUTO_LINK:BOOL=TRUE \
        -D PREPARE_CLASS_FILES:BOOL=TRUE  -D CREATE_PYTHON:BOOL=TRUE .
  make
  
  ###### BUG CHECK: should at this point recompile without the shared libraries? problems with ipopt

  # Build yarp.jar
  mkdir -p yarp
  for f in $(find ${YARP_DIR} -name *.o); do cp -al ${f} yarp; done
  jar cvf yarp.jar yarp
  rm -rf yarp/
  
  # Copy everything to interfaces directory - hard links for optimization
  rm -rf ${INTDIR}
  mkdir -p ${INTDIR} 
  cp -al ${SWIG_DIR}/{yarp.jar,libjyarp.so,LoadYarp.class,YarpImageHelper.class,_yarp.so} ${INTDIR}
  
  echo " YARP interfaces built"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


compile_icubsim(){

  echo " Compiling iCub simulator..."

  cd ${ICUB_ROOT}

  # Give default configuration
  cp ${ICUB_ROOT}/app/ICUB_ROOT.ini ${ICUB_ROOT}
  
  cmake -D ICUB_USE_IPOPT:BOOL=${IPOPT} .
  make

  echo " iCub simulator compiled"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


install_icubsim(){

  echo " Installing iCub simulator system-wide..."

  cd ${ICUB_ROOT}
  sudo make install

  echo " iCub simulator installed"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


activate_icub_parts(){
# This is just a fix for the 1.1.0 bug

  echo " Activating iCub parts..."

  cd ${ICUB_ROOT}
  ln -s app/ICUB_ROOT.ini .
  ACTPARTS=${ICUB_ROOT}/app/simConfig/conf/iCub_parts_activation.ini
  mv ${ACTPARTS}{,.BACKUP}
  echo -e "/// CUSTOM SETUP - see \$file.BACKUP for defaults
  \n[SETUP]\nelevation off \n\n[PARTS]\nlegs off
  torso on
  left_arm  on  \nleft_hand  on
  right_arm on  \nright_hand on
  head      on  \nfixed_hip  on 
  \n[SENSORS]   \npressure  off
  \n[VISION]    \ncam        on 
  \n[RENDER]    \nobjects   off    \ncover on
  " > ${ACTPARTS}
  ln -s ${ACTPARTS} ..

  echo " iCub parts activated"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


install_vskin(){

  echo " Installing virtual skin..."
  
  # Install dependancies
  sudo ${APT} -y install -y qt4-qmake libqt4-dev libglut3-dev
  rm -rf ${VSKIN_DIR}

  # Get and extract vskin and FreeSOLID  
  mkdir -p ${VSKIN_DIR}
  cd ${VSKIN_DIR}
  tar xjf ${VSKIN} -C . &
  wget http://www.dtecta.com/files/solid-${LIBSOLID_V}.tgz
  tar xzf solid-${LIBSOLID_V}.tgz

  # Compiling and installing libsolid
  export SOLID_DIR=${VSKIN_DIR}/solid-${LIBSOLID_V}
  cd ${SOLID_DIR}
  ./configure
  make && sudo make install && sudo ldconfig

  # Compiling virtual skin
  cd ${VSKIN_DIR}
  cmake -D QT_QMAKE_EXECUTABLE:STRING="/usr/bin/qmake-qt4" .
  make install
  
  echo " Virtual skin compiled"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}


run_test(){

  echo " Launching system test..."

  if [ -f ${CONTROLSCRIPT} ]; then  
    ${CONTROLSCRIPT} start
    echo "Close the simulator window to finish"
    if [ -f ${VSKIN} ]; then  
      ${VSKIN_DIR}/bin/ReflexDemo --visualize --robot ${VSKIN_DIR}/xml/icubSim.xml &
      sleep 1
    fi
    ${CONTROLSCRIPT} dance 5
    ${CONTROLSCRIPT} stop
  else
    yarp server &
    sleep 1 # wait for yarp server to initialize
    iCub_SIM
    killall yarp
  fi
  
  echo " Run test completed"
  if ${debug}; then echo " Press enter to continue..." && read; fi
}



###### CONTROL SEQUENCE

### ALWAYS run these two
gain_root_access
set_environmental_variables

### You can deactivate the parts you do not want
add_environmental_variables_to_bashrc
if ${GET_XTRA_SW}; then   download_from_giuse; fi
download_and_extract_software
if [ -f ${LEOSIM} ]; then   copy_over_LEO_sim  ; fi
activate_repositories
install_dependancies
compile_yarp
install_yarp
if ${INTERFS}; then 
  install_interfaces_dependancies && \
  build_yarp_interfaces &     # sent in background for optimization
fi
compile_icubsim
install_icubsim
if [ ${ICUB_V} == "1.1.0" ]; then   activate_icub_parts ; fi
if [ -f ${VSKIN} ]; then  install_vskin  ; fi


# DONE!

if [ -f ${VSKIN} ]; then  mv ${VSKIN} ${DIR}  ; fi
if [ -f ${LEOSIM} ]; then  mv ${LEOSIM} ${DIR}  ; fi

run_test


echo "INSTALLATION FINISHED!"
if ${debug}; then echo " Press enter to continue..." && read; fi

exit 0





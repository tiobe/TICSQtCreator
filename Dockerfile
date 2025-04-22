ARG DISTRO='ubuntu'
ARG VERSION='20.04'
FROM ${DISTRO}:${VERSION}
ARG DEBIAN_FRONTEND=noninteractive

# Build arguments.
ARG UPDATE_PACKAGES='apt-get update'
ARG INSTALL_PACKAGES='apt-get install -y'
ARG CLEAN_PACKAGES='rm -rf /var/lib/apt/lists/*'

# Install necessary tools for building.
RUN ${UPDATE_PACKAGES}  && \
    ${INSTALL_PACKAGES} wget build-essential cmake ninja-build git p7zip-full python-is-python3 python2

# Install necessary dependencies for building Qt and Qt Creator
RUN ${INSTALL_PACKAGES} libgl1-mesa-dev libvulkan-dev libxcb-xinput-dev libxcb-xinerama0-dev libxkbcommon-dev libxkbcommon-x11-dev libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-xkb1 libxcb-randr0 libxcb-icccm4 && \
    ${INSTALL_PACKAGES} libglib2.0-dev libsm-dev libxrender-dev libfontconfig1-dev libxext-dev libfreetype6-dev libx11-dev libxcursor-dev libxfixes-dev libxft-dev libxi-dev libxrandr-dev libgl-dev libglu-dev && \
    ${INSTALL_PACKAGES} libxml2 libxml2-dev && \
    ${INSTALL_PACKAGES} gperf bison flex

# Install Gcc of a specific version.
ARG GCC_VERSION='7'
RUN ${UPDATE_PACKAGES} && \
    ${INSTALL_PACKAGES} g++-${GCC_VERSION} && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} ${GCC_VERSION} --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION}

# Install LLVM of a specific version.
ARG LLVM_ARCHIVE='libclang-release_80-based-linux-Ubuntu16.04-gcc5.3-x86_64.7z'
ARG LLVM_ARCHIVE_URL=https://download.qt.io/development_releases/prebuilt/libclang/${LLVM_ARCHIVE}
RUN mkdir llvm && \
    cd llvm && \
    mkdir llvm-pkg && \
    wget ${LLVM_ARCHIVE_URL} && \
    7z x ${LLVM_ARCHIVE} -o/llvm/llvm-pkg

# Build and install Qt 5.*.*
ARG QT_ARCHIVE_PKG='qt-everywhere-src-5.12.8'
ARG QT_ARCHIVE_FILE=${QT_ARCHIVE_PKG}.tar.xz
ARG QT_ARCHIVE_URL='https://download.qt.io/archive/qt/5.12/5.12.8/single/qt-everywhere-src-5.12.8.tar.xz'
RUN mkdir qt && \
    cd qt && \
    wget ${QT_ARCHIVE_URL}
RUN cd qt && \
    tar xf ${QT_ARCHIVE_FILE}
      
ARG QT_CONFIGURE_CMD='./configure -opensource -confirm-license -nomake tests -nomake examples -prefix /qt/qtbase -skip qt3d -skip qtgamepad -skip qtspeech -skip qtdoc -skip qtsensors -skip qtdatavis3d -skip qtcharts -skip qtandroidextras -skip qtmacextras -skip qtvirtualkeyboard -skip qtcharts -skip qtpurchasing -skip qtserialbus -skip qtserialport -skip qtlocation -skip qtconnectivity -skip qttranslations'
ARG SKIP_QUICK3D='-skip qtquick3d' 
RUN cd /qt && \
    mkdir qtbase && \
    cd /qt/${QT_ARCHIVE_PKG} && \
    if [ -d /qt/${QT_ARCHIVE_PKG}/qtquick3d ]; then \
      ${QT_CONFIGURE_CMD} ${SKIP_QUICK3D}; \
    else \
      ${QT_CONFIGURE_CMD}; \
    fi;
     
RUN cd /qt/${QT_ARCHIVE_PKG} && \
    export LLVM_HOME=/llvm/llvm-pkg/libclang && \
    export PATH=$LLVM_HOME/bin:$PATH && \
    export LD_LIBRARY_PATH=$LLVM_HOME/lib:$LD_LIBRARY_PATH && \
    make -j4 > qtbuildoutput.txt 2>qtbuilderror.txt
RUN cd /qt/${QT_ARCHIVE_PKG} && \
    make install

# Check out the correct Qt Creator branch to build with
ARG QT_CREATOR_VERSION='4.11.0'
ARG QT_CREATOR_TAG=v${QT_CREATOR_VERSION}
RUN git clone https://github.com/qt-creator/qt-creator.git && \
    cd qt-creator && \
    git checkout -b ${QT_CREATOR_VERSION} ${QT_CREATOR_TAG}

# Build Qt Creator 4.*.*
RUN mkdir qt-creator-build && \
    cd /qt-creator-build && \
    export PATH=$PATH:/qt/qtbase/bin && \
    export LLVM_INSTALL_DIR=/llvm/llvm-pkg/libclang && \
    qmake -r ../qt-creator && \
    make -j8 > qtcreatorbuildoutput.txt 2>qtcreatorbuilderror.txt

# Build TICS Qt Plugin
ARG QT_CREATOR_DEFAULT_VERSION='4.12.2'
ARG TICS_QT_GIT_BRANCH='main'
RUN git clone https://github.com/tiobe/TICSQtCreator.git && \
    cd TICSQtCreator && \
    git checkout ${TICS_QT_GIT_BRANCH} && \
    sed -i s/${QT_CREATOR_DEFAULT_VERSION}/${QT_CREATOR_VERSION}/g TICSQtCreator.json && \
    export PATH=$PATH:/qt/qtbase/bin && \
    qmake IDE_SOURCE_TREE=/qt-creator IDE_BUILD_TREE=/qt-creator-build && \
    make
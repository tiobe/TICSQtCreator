# TICSQtCreator
TICS plugin for Qt Creator

This plugin allows you to do a TICS Analysis on your code from within QtCreator.

To learn more about TICS please visit the documentation on [TIOBE Software's](https://tiobe.com/tics/tics-framework/) website

# Qt Creator Extension Development and Setup

## Installation on Ubuntu 18.04 with GCC 7.5.0

For other supported environments see [here](https://doc.qt.io/qtcreator/creator-os-supported-platforms.html)

Install QT
---------------------------

- Download qt offline installer from https://www.qt.io/offline-installers
- Make it executable
    sudo chmod +x qt-opensource-linux-x64-5.14.2.run
- Run the installer script
 ./qt-opensource-linux-x64-5.14.2.run
- Once the installation starts, and you get a selection of options, select Qt - u can deselect some of the components but Qt webengine Qt script and compiler gcc are important

Build Qt Creator from Sources
-------------------------
- Get Qt creator sources
    -git clone https://code.qt.io/cgit/qt-creator/qt-creator.git
- Checkout the latest or the preffered branch
  - <code>git checkout -b  4.12.1 v4.12.1</code>
  - <code>mv qt-creator qt-creator-4.12.1</code>
- Create the build directory next to the Qt Creator sources (this is important the build may fail if the source and build directories are not next to each other.)
  - <code>cd ..</code>
  - <code>mkdir build-qtcreator-Desktop_Qt_5_14_2_GCC_64bit-Debug</code>

  Note: the name of the folder is the one that would be assigned if you use the desktop version of qt creator to build with the specific verions of QT, compiler and platform mentioned. Although you are free to use qt-creator-build as mentioned [here](https://wiki.qt.io/Building_Qt_Creator_from_Git), this naming scheme can be helpful for identifying multiple builds)
-cd build-qtcreator-Desktop_Qt_5_14_2_GCC_64bit-Debug
- Make sure qmake is in your path
  - <code>export PATH=$PATH:/home/beza/Qt5.14.2/5.14.2/gcc_64/bin/</code>
- Build the tool
  - <code>qmake -r <QtCreatorSourceDir></code>
  - <code>make</code>
  - If you encounter an error with some libs not found, you have to install them using the package manager of your system
  - for e.g.
    - error with -lGL not found
    - <code>sudo apt install libglfw3-dev</code>
- wait about 30 mins
- when done run <code>bin/qtcreator </code>
- Now you have a working build of QtCreator (hopefully)

Building the Qt Creator Plugin
--------------------------------------
- For building the plugin you need Qt Creator installed(This Qt creator should be using the same version of Qt and compiler version as the one you build from sources)
- In the plugin's .pro file update the location of the QT Creator source location and the QT Creator build location that you created before
 <pre>
    ## Either set the IDE_SOURCE_TREE when running qmake,
    ## or set the QTC_SOURCE environment variable, to override the default setting
    isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
    isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "../qtcreator-src/qt-creator-4.12.1"

    ## Either set the IDE_BUILD_TREE when running qmake,
    ## or set the QTC_BUILD environment variable, to override the default setting
    isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
    isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "../qtcreator-build/build-qtcreator-Desktop_Qt_5_14_2_GCC_64bit-Debug"
</pre>
- when you build the plugin it will be deployed to the Qt Creator build you specified as IDE_BUILD_TREE
- another (better) option is to set the <code>QTC_BUILD</code> and QTC_SOURCE</code> system environment variables
- To test, run your Qt Creator build from <code><YOUR_QT_CREATOR_BUILD_DIR>/bin/qtcreator</code>

Using Dockerfile to Automate the Qt Creator Plugin Build
--------------------------------------------------------
<<<<<<< HEAD
You can use the provided Dockerfile to build and get a working TICS Qt Creator Plugin artifact for a specific target platform.
Note that this Dockerfile is written for target platform Qt Creator 4.x.x based on Qt 5.x.x. Target platforms with different major versions than those may require different build steps.
=======
You can use the provided Dockerfile to build and get a working TICS Qt Creator Plugin artifact for specific target platforms.
>>>>>>> daf1fbe8fb50a4012e5b596a0b8def70b0f92c5d
The arguments used for parameterizing specific target platforms and their values are shown below:

<pre>
DISTRO=ubuntu
VERSION=20.04
GCC_VERSION=7
<<<<<<< HEAD
LLVM_ARCHIVE=libclang-release_80-based-linux-Ubuntu16.04-gcc5.3-x86_64.7z
=======
LLVM_ARCHIVE=libclang-release_80-based-linux-Ubuntu16.04-gcc5.3-x86_64.7z  
>>>>>>> daf1fbe8fb50a4012e5b596a0b8def70b0f92c5d
QT_ARCHIVE_PKG=qt-everywhere-src-5.12.8
QT_ARCHIVE_URL=https://download.qt.io/archive/qt/5.12/5.12.8/single/qt-everywhere-src-5.12.8.tar.xz
QT_CREATOR_VERSION=4.11.0
TICS_QT_GIT_BRANCH=main
</pre>

You can change the values of these arguments based on your desired target platforms when building the Dockerfile. The following command shows an example of building the TICS Qt Creator plugin for target platform Qt Creator 4.12.2 based on Qt 5.14.2:

```
docker build --build-arg GCC_VERSION=7 --build-arg QT_ARCHIVE_PKG=qt-everywhere-src-5.14.2 --build-arg QT_ARCHIVE_URL=https://download.qt.io/archive/qt/5.14/5.14.2/single/qt-everywhere-src-5.14.2.tar.xz  --build-arg QT_CREATOR_VERSION=4.12.2 -t ticsqtcreator:4-12-2 .
```
The command above will create an image with the tag `ticsqtcreator:4-12-2`. The TICS Qt Creator plugin artifact will be located in `/qt-creator-build/lib/qtcreator/plugins/libTICSQtCreator.so`. To get the artifact from the created image to the host file system, you can first run a container from the created image, then perform a docker copy as shown in the following commands examples:

```
docker run -it -d --name ticsqtcreator fddf7a31202a
docker cp ticsqtcreator:/qt-creator-build/lib/qtcreator/plugins/libTICSQtCreator.so /home/leila/Development/QtDocker/v4
```

<<<<<<< HEAD

=======
>>>>>>> daf1fbe8fb50a4012e5b596a0b8def70b0f92c5d
## References
- [Qt wiki page on building QT creator from sources](https://wiki.qt.io/Building_Qt_Creator_from_Git)
- [A working example of Qt Creator with tutorial](http://blog.davidecoppola.com/2019/12/how-to-create-a-qt-creator-plugin/)
- [Qt creator extension documentation](https://doc.qt.io/qtcreator-extending/first-plugin.html)
- [Qt creator API](https://doc.qt.io/qtcreator-extending/qtcreator-api.html)
- [Qt compatibility rules](https://doc.qt.io/qtcreator-extending/coding-style.html#binary-and-source-compatibility)
- [Qt Creator Supported Platforms](https://doc.qt.io/qtcreator/creator-os-supported-platforms.html)
- [Qt Creator Cppcheck plugin source code](https://github.com/OneMoreGres/qtc-cppcheck/blob/master/src/QtcCppcheckPlugin.cpp)

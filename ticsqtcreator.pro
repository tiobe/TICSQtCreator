DEFINES += TICSQTCREATOR_LIBRARY
QMAKE_CXXFLAGS += "-fno-sized-deallocation"

# TICSQtCreator files

SOURCES +=         ticsqtcreatorplugin.cpp \
    ticsoutputpane.cpp

HEADERS +=         ticsqtcreatorplugin.h         ticsqtcreator_global.h         ticsqtcreatorconstants.h \
    ticsoutputpane.h

RESOURCES +=       ticsqtcreator.qrc

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "../qtcreator-src/qt-creator-4.12.2"

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "../qtcreator-build/build-qtcreator4.12.2-Qt_5_14_2_GCC_64bit-Debug"

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = TICSQtCreator
QTC_LIB_DEPENDS +=     # nothing here at this time

QTC_PLUGIN_DEPENDS +=  coreplugin\
                       projectexplorer

QTC_PLUGIN_RECOMMENDS +=     # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

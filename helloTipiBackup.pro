TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

QT += network xml

RC_FILE = helloTipiBackup.rc

# Icon Composer
ICON = helloTipiBackup.icns

RESOURCES = helloTipiBackup.qrc

HEADERS += helloTipiBackupMainWindow.H \
           helloTipiBackupEngine.H \
           helloTipiSite.H \
           helloTipiAlbum.H \
           helloTipiPicture.H

FORMS += helloTipiBackupMainWindow.ui

TRANSLATIONS += helloTipiBackupEN.ts

SOURCES += helloTipiBackupMain.cxx \
           helloTipiBackupMainWindow.cxx \
           helloTipiBackupEngine.cxx \
           helloTipiSite.cxx \
           helloTipiAlbum.cxx \
           helloTipiPicture.cxx

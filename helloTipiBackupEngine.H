#ifndef _HelloTipiEngine_H_
#define _HelloTipiEngine_H_

#include <QString>
#include <QList>
#include <QStringList>
#include <QByteArray>

#include <QHttp>

#include "helloTipiSite.H"
#include "helloTipiAlbum.H"

#define API_HOST    "apitxt.hellotipi.com"
#define URL_SITES   "/account/my_sites.xml"

class helloTipiBackupEngine
{
    public:
        static QList<helloTipiSite*> getSitesFromXml(QByteArray);
        static QList<helloTipiAlbum*> getAlbumsFromXml(QByteArray);
        static QList<helloTipiPicture*> getPicturesFromXml(QByteArray);

        static bool createFileDirectoryHierarchy(QString, QList<helloTipiSite*>);

        static bool verifyDirectory(QString);
        static bool verifyFile(QString);
};

#endif

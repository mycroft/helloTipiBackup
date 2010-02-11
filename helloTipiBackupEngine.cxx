
#include <QUrl>

#include <QDir>

#include <QDomDocument>
#include <QDomElement>

#include <QHash>

#include <QDebug>

#include "helloTipiBackupEngine.H"

QList<helloTipiSite*> helloTipiBackupEngine::getSitesFromXml(QByteArray contents)
{
    QDomDocument doc;
    QDomElement root, child, element;

    QList<helloTipiSite*> sites;
    helloTipiSite *tmpSite;

    doc.setContent(contents, FALSE);

    root = doc.documentElement();
    child = root.firstChild().toElement();

    while(!child.isNull())
    {
        if(child.tagName() == "item")
        {
            element = child.firstChild().toElement();
            tmpSite = new helloTipiSite();

            while(!element.isNull())
            {
                if(element.tagName() == "name")
                    tmpSite->setName(element.text());

                if(element.tagName() == "title")
                    tmpSite->setTitle(element.text());

                if(element.tagName() == "role")
                    tmpSite->setRole(element.text());

                if(element.tagName() == "read_only")
                    tmpSite->setReadOnly(element.text().toInt());

                if(element.tagName() == "image_small")
                    tmpSite->setImageSmall(element.text());

                element = element.nextSibling().toElement();
            }

            sites.append(tmpSite);
        }
        child = child.nextSibling().toElement();
    }

    return sites;
}

QList<helloTipiAlbum*> helloTipiBackupEngine::getAlbumsFromXml(QByteArray contents)
{
    QDomDocument doc;
    QDomElement root, child, element;

    QList<helloTipiAlbum*> albums;
    helloTipiAlbum *tmpAlbum;

    doc.setContent(contents, FALSE);

    root = doc.documentElement();
    child = root.firstChild().toElement();

    while(!child.isNull())
    {
        if(child.tagName() == "item")
        {
            tmpAlbum = new helloTipiAlbum();

            element = child.firstChild().toElement();
            while(!element.isNull())
            {
                if(element.tagName() == "name")
                    tmpAlbum->setName(element.text());

                if(element.tagName() == "id")
                    tmpAlbum->setId(element.text().toInt());

                element = element.nextSibling().toElement();
            }
            albums.append(tmpAlbum);
        }
        child = child.nextSibling().toElement();
    }

    return albums;
}

QList<helloTipiPicture*> helloTipiBackupEngine::getPicturesFromXml(QByteArray contents)
{
    QList<helloTipiPicture*> pictures;
    helloTipiPicture* tmpPicture;

    QDomDocument doc;
    QDomElement root, child, element;

    qDebug() << contents.data();

    doc.setContent(contents, FALSE);

    root = doc.documentElement();
    child = root.firstChild().toElement();

    while(!child.isNull())
    {
        if(child.tagName() == "item")
        {
            element = child.firstChild().toElement();

            tmpPicture = new helloTipiPicture();

            while(!element.isNull())
            {
                if(element.tagName() == "id")
                    tmpPicture->setId(element.text().toInt());

                if(element.tagName() == "url_small")
                    tmpPicture->setUrlSmall( element.text() );

                if(element.tagName() == "url_medium")
                    tmpPicture->setUrlMedium( element.text() );

                if(element.tagName() == "url_large")
                    tmpPicture->setUrlLarge( element.text() );

                if(element.tagName() == "date")
                    tmpPicture->setDate( QDateTime::fromString(element.text(), QString("yyyy-MM-dd HH:mm:ss")) );

                if(element.tagName() == "exif_date")
                    tmpPicture->setExifDate( QDateTime::fromString(element.text(), QString("yyyy-MM-dd HH:mm:ss")) );

                if(element.tagName() == "user")
                    tmpPicture->setUser( element.text() );

                element = element.nextSibling().toElement();
            }

            pictures.append(tmpPicture);
        }
        child = child.nextSibling().toElement();
    }

    return pictures;
}

bool helloTipiBackupEngine::createFileDirectoryHierarchy(QString root, QList<helloTipiSite*> sites)
{
    QDir rootDir(root);
    bool ret = true;

    if(rootDir.exists() != true)
    {
        if( (ret = rootDir.mkpath(root)) == false)
            return ret;
    }

    for(int i = 0; i < sites.count(); ++i)
    {
        QString actualPath = root + QDir::separator() + sites.at(i)->name();
        QFileInfo di(actualPath);
        if(di.exists() == true)
        {
            if(di.isDir() == false)
                return false;
        } else {
            rootDir.mkdir(actualPath);
        }
        for(int j = 0; j < sites.at(i)->albums().count(); ++j)
        {
            QString actualSubPath = actualPath + QDir::separator() + sites.at(i)->albums().at(j)->alias();
            QFileInfo subDi(actualSubPath);
            if(subDi.exists() == true)
            {
                if(subDi.isDir() == false)
                    return false;
            } else {
                rootDir.mkdir(actualSubPath);
            }
        }
    }

    return true;
}

bool helloTipiBackupEngine::verifyDirectory(QString directory)
{
    QDir dir(directory);

    if(dir.exists() != true)
    {
        return false;
    }

    return true;
}

bool helloTipiBackupEngine::verifyFile(QString filePath)
{
    QFileInfo fp(filePath);

    return fp.exists();
}



#ifndef _HELLOTIPISITE_H_
#define _HELLOTIPISITE_H_

#include <QString>
#include <QList>

#include "helloTipiAlbum.H"

class helloTipiSite
{
private:
    QString     _name;
    QString     _title;
    QString     _role;
    int         _read_only;
    QString     _image_small; /* Url */

    QList<helloTipiAlbum*>  _albums;

public:
    helloTipiSite(void);
    helloTipiSite(QString, QString, QString, int);

    inline QString name(void) { return _name; };
    inline QString title(void) { return _title; };
    inline QString role(void) { return _role; };
    inline int readOnly(void) { return _read_only; };
    inline QString imageSmall(void) { return _image_small; };

    inline void setName(QString name) { _name = name; };
    inline void setTitle(QString title) { _title = title; };
    inline void setRole(QString role) { _role = role; };
    inline void setReadOnly(int read_only) { _read_only = read_only; };
    inline void setImageSmall(QString image_small) { _image_small = image_small; };

    inline QList<helloTipiAlbum*> albums(void) { return _albums; };
    void setAlbums(QList<helloTipiAlbum*>);

    inline int albumsCount(void) { return _albums.size(); };
    int picturesCount(void);

};

#endif

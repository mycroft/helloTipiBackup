#ifndef _HELLOTIPIALBUM_H_
#define _HELLOTIPIALBUM_H_

#include <QString>
#include <QList>

#include "helloTipiPicture.H"

class helloTipiAlbum
{
private:
    QString     _name;
    QString     _alias;
    int         _id;

    QList<helloTipiPicture*> _pictures;

public:
    helloTipiAlbum(void);
    ~helloTipiAlbum(void);

    inline QString name(void) { return _name; };
    inline int id(void) { return _id; };
    inline QString alias(void) { return _alias; };

    inline QList<helloTipiPicture*> pictures(void) { return _pictures; };

    inline void setName(QString name) { _name = name; };
    inline void setId(int id) { _id = id; };
    inline void setAlias(QString alias) { _alias = alias; };

    void setPictures(QList<helloTipiPicture*>);

    inline int picturesCount(void) { return _pictures.count(); };
};

#endif

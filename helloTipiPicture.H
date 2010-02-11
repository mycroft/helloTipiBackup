#ifndef _HELLOTIPIPICTURE_H_
#define _HELLOTIPIPICTURE_H_

#include <QString>

#include <QDateTime>

class helloTipiPicture
{
private:
    int         _id;
    QString     _url_small;
    QString     _url_medium;
    QString     _url_large;
    QDateTime   _date;
    QDateTime   _exif_date;
    QString     _user;

public:
    helloTipiPicture(void);
    ~helloTipiPicture(void);

    inline int id(void) { return _id; };
    inline QString urlSmall(void) { return _url_small; };
    inline QString urlMedium(void) { return _url_medium; };
    inline QString urlLarge(void) { return _url_large; };
    inline QString user(void) { return _user; };
    inline QDateTime date(void) { return _date; };
    inline QDateTime exifDate(void) { return _exif_date; };

    inline void setId(int id) { _id = id; };
    inline void setUrlSmall(QString urlSmall) { _url_small = urlSmall; };
    inline void setUrlMedium(QString urlMedium) { _url_medium = urlMedium; };
    inline void setUrlLarge(QString urlLarge) { _url_large = urlLarge; };
    inline void setUser(QString user) { _user = user; };
    inline void setDate(QDateTime date) { _date = date; };
    inline void setExifDate(QDateTime exif_date) { _exif_date = exif_date; };

};

#endif

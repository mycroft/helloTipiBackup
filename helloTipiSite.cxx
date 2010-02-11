
#include "helloTipiSite.H"

helloTipiSite::helloTipiSite(void)
{
}

helloTipiSite::helloTipiSite(QString name, QString title, QString role, int read_only)
    : _name(name), _title(title), _role(role), _read_only(read_only)
{
    return;
}

void helloTipiSite::setAlbums(QList<helloTipiAlbum*> albums)
{
    for(int i = 0; i < albums.size(); ++i)
    {
        _albums.append(albums.at(i));
    }

    return ;
}

int helloTipiSite::picturesCount(void)
{
    int n = 0;

    for(int i = 0; i < _albums.size(); ++i)
    {
        n += _albums.at(i)->picturesCount();
    }

    return n;
}



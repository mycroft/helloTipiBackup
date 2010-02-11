
#include "helloTipiAlbum.H"

helloTipiAlbum::helloTipiAlbum(void)
{
}

helloTipiAlbum::~helloTipiAlbum(void)
{
}

void helloTipiAlbum::setPictures(QList<helloTipiPicture*> pictures)
{
    for(int i = 0; i < pictures.size(); ++i)
    {
        _pictures.append( pictures.at(i) );
    }

    return;
}

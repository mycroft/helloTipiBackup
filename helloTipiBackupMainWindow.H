#ifndef _HelloTipiBackupMainWindow_H_
#define _HelloTipiBackupMainWindow_H_

#include <QHttp>

#include "helloTipiBackupEngine.H"

#include "helloTipiSite.H"
#include "helloTipiAlbum.H"

#include "ui_helloTipiBackupMainWindow.h"

#define DEFAULT_STATUSBAR_TIME  60 * 1000

#ifndef QT_NO_OPENSSL
 #define _USE_SSL
#else
 #error "You should recompile Qt in order to support OpenSSL."
#endif

#define API_HOST    "apitxt.hellotipi.com"
#ifdef _USE_SSL
#define API_PORT    443
#else
#define API_PORT    80
#endif

#define URL_SITES   "/account/my_sites.xml"
#define URL_ALBUMS  "/photo/site_albums.xml"
#define URL_PICTURES "/photo/album_photos.xml"

class helloTipiBackupMainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

private:
    QHttp *currentQuery;
    QEventLoop *m_eventLoop;

    QHash<int, QStringList> queries;

    QList<helloTipiSite*> sites;
    QList<helloTipiAlbum*> albums;

    QList<helloTipiSite*> sitesToBackup;

    bool isDownloadDone;

    int numberFileDone, numberFileTotal;
    QDateTime startTime;
    int totalBytesDone;

public:
    helloTipiBackupMainWindow(QMainWindow *parent = NULL);
    ~helloTipiBackupMainWindow();

    void startBlockingQuery(QString , QHash<QString, QString>);

private:
    void readSettings(void);
    void writeSettings(void);

public slots:
    void loginButtonPushed(void);
    void siteChooserButtonPushed(void);
    void backupButtonPushed(void);
    void changeDirectoryButtonPushed(void);
    void endDownloadButtonPushed(void);

    void backToSiteButtonPushed(void);

    void blockingQueryFinished(bool);

    void downloadRequestStarted(int);
    void downloadRequestFinished(int, bool);
    void downloadDataReadProgress(int, int);
    void downloadDone(bool);

    void aboutLabelPushed(QString);

signals:
    void singleQueryDone(void);
};

#endif

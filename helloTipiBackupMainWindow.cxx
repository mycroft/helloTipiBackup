
#include <math.h>

#include <QApplication>
#include <QMessageBox>
#include <QUrl>

#include <QFileDialog>

#include <QFile>

#include <QSettings>

#include <QDebug>

#include "helloTipiBackupMainWindow.H"

#define DEBUG

helloTipiBackupMainWindow::helloTipiBackupMainWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    currentQuery = new QHttp(QString(API_HOST), QHttp::ConnectionModeHttp, API_PORT);

    connect(loginButton, SIGNAL(clicked(void)), this, SLOT(loginButtonPushed(void)));
    connect(siteChooserButton, SIGNAL(clicked(void)), this, SLOT(siteChooserButtonPushed(void)));
    connect(backupButton, SIGNAL(clicked(void)), this, SLOT(backupButtonPushed(void)));
    connect(endDownloadButton, SIGNAL(clicked(void)), this, SLOT(endDownloadButtonPushed(void)));

    connect(changeDirectoryButton, SIGNAL(clicked(void)), this, SLOT(changeDirectoryButtonPushed(void)));

    connect(aboutLabel, SIGNAL(linkActivated(QString)), this, SLOT(aboutLabelPushed(QString)));

    connect(backToSiteButton, SIGNAL(clicked(void)), this, SLOT(backToSiteButtonPushed(void)));

    installationPathLabel->setText( QDir::homePath() );

    readSettings();

    QPixmap pixmap;
    pixmap.load(":/pictures/banner.png");
    headerLabel->setPixmap(pixmap);

    stackedWidget->setCurrentIndex( 0 );
}

helloTipiBackupMainWindow::~helloTipiBackupMainWindow()
{
    return ;
}

void helloTipiBackupMainWindow::loginButtonPushed(void)
{
    QString message;
    QStringList siteList;
    QHash<QString, QString> optArgs;

    QList<helloTipiPicture*> pictures;

    writeSettings();

    currentQuery->setUser(loginEdit->text(), passwordEdit->text());

    startBlockingQuery(QString(URL_SITES) , QHash<QString, QString>());

    /* Read HTTP status */
    if(currentQuery->lastResponse().statusCode() == 200)
    {
        /* We are log'ed in ! */
        stackedWidget->setCurrentIndex( 1 );

        siteChooserButton->setEnabled( false );
        siteChooserButton->setText( tr("Merci de patienter...") );

        // Retrieve sites and fill the site list
        sites = helloTipiBackupEngine::getSitesFromXml(currentQuery->readAll());

        QString baseLabelText;

        if(sites.count() > 1)
        {
            baseLabelText = tr("Chargement de la liste des albums disponibles sur vos %1 sites.\n").arg(sites.count());
        }
        else
        {
            baseLabelText = tr("Chargement de la liste des albums disponibles sur votre site.\n");
        }
        label->setText(baseLabelText);

        if(sites.empty())
        {
            QMessageBox::warning(this, tr("Pas de site"), tr("Pas de site à sauvegarder n'a été trouvé."));
            close();
        }

        for(int i = 0; i < sites.size(); ++i)
        {
            optArgs["site"] = sites.at(i)->name();
            startBlockingQuery(QString(URL_ALBUMS), optArgs);

            albums = helloTipiBackupEngine::getAlbumsFromXml(currentQuery->readAll());

            for(int j = 0; j < albums.size(); ++j)
            {
                label->setText(baseLabelText + tr("Chargement des photos du site \"%1\"").arg(albums.at(j)->name()));
                /* Check other albums to see if there is a album with the same name */
                int z = 0;
                for(int k = 0; k < j; k ++)
                {
                    if(albums.at(k)->name() == albums.at(j)->name())
                        z++;
                }
                if(z == 0)
                {
                    albums.at(j)->setAlias(albums.at(j)->name());
                }
                else
                {
                    albums.at(j)->setAlias(QString("%1 (%2)").arg(albums.at(j)->name()).arg(QString::number(z)));
                }
                optArgs["album_id"] = QString::number(albums.at(j)->id());

                startBlockingQuery(QString(URL_PICTURES), optArgs);

                pictures = helloTipiBackupEngine::getPicturesFromXml(currentQuery->readAll());

                albums.at(j)->setPictures( pictures );
            }

            sites.at(i)->setAlbums(albums);
        }

        label->setText(tr("Choix de la sauvegarde\nSélectionnez dans la liste ci dessous les sites à sauvegarder :"));

        siteChooserButton->setEnabled( true );
        siteChooserButton->setText( tr("Mon choix est fait !") );

        for(int i = 0; i < sites.size(); ++i)
        {
            QString title = sites.at(i)->title();
            title += tr(" (%1 album%3, %2 photo%4)").arg(sites.at(i)->albumsCount()).arg(sites.at(i)->picturesCount())
                                                    .arg((sites.at(i)->albumsCount() > 1)?QString("s"):QString())
                                                    .arg((sites.at(i)->picturesCount() > 1)?QString("s"):QString());
            siteListWidget->addItem(title);
        }

        for(int i = 0; i < siteListWidget->count(); i ++)
        {
            QListWidgetItem *item = siteListWidget->item(i);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);
        }
    }
    else if(currentQuery->lastResponse().statusCode() == 401)
    {
        QMessageBox::warning(this, tr("Connexion"),
                             tr("Impossible de s'authentifier sur HelloTipi\nVérifiez que votre pseudo et votre mot de passe."),
                             QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Connexion"),
                             tr("Impossible de créer une connexion sur HelloTipi\nMerci de réessayer ultérieurement\nou bien de contacter le support technique."),
                             QMessageBox::Ok);
    }

    qDebug() << "code: " << currentQuery->lastResponse().statusCode();
    qDebug() << currentQuery->lastResponse().reasonPhrase();
    qDebug() << currentQuery->readAll();

    return ;
}

void helloTipiBackupMainWindow::siteChooserButtonPushed(void)
{
    sitesToBackup.clear();

    /* Retrieving all sites we want to backup */
    for(int i = 0; i < siteListWidget->count(); ++i)
    {
        QListWidgetItem *item = siteListWidget->item(i);
        if(item->checkState() == Qt::Checked)
        {
            sitesToBackup.append(sites.at(i));
            qDebug() << "Will retrieve site: " << sites.at(i)->name();
        }
    }

    stackedWidget->setCurrentIndex( 2 );
}

void helloTipiBackupMainWindow::backupButtonPushed(void)
{

    isDownloadDone = false;

    if(helloTipiBackupEngine::verifyDirectory( installationPathLabel->text() ) == false)
    {
        QMessageBox::warning(this, tr("Dossier incorrect"), tr("Il est nécessaire de sélectionner\nun dossier valider pour la sauvegarde."));
        return;
    }

    /* lets write settings */
    writeSettings();

    stackedWidget->setCurrentIndex( 3 );
    /* Create directory hierarchy */
    helloTipiBackupEngine::createFileDirectoryHierarchy(installationPathLabel->text(), sitesToBackup);

    /* Launch downloads */
    connect(currentQuery, SIGNAL(requestStarted(int)), this, SLOT(downloadRequestStarted(int)));
    connect(currentQuery, SIGNAL(requestFinished(int, bool)), this, SLOT(downloadRequestFinished(int, bool)));
    connect(currentQuery, SIGNAL(dataReadProgress(int, int)), this, SLOT(downloadDataReadProgress(int, int)));
    connect(currentQuery, SIGNAL(done(bool)), this, SLOT(downloadDone(bool)));

    queries.clear();

    numberFileDone = numberFileTotal = 0;

    for(int i = 0; i < sitesToBackup.count(); ++i)
    {
        for(int j = 0; j < sitesToBackup.at(i)->albums().count(); ++j)
        {
            for(int k = 0; k < sitesToBackup.at(i)->albums().at(j)->pictures().count(); ++k)
            {
                QUrl pictureUrl = QUrl(sitesToBackup.at(i)->albums().at(j)->pictures().at(k)->urlLarge());

                QStringList splist = pictureUrl.path().split("/");
                QString fileName = splist.last();
                int id;

                // Before launching the backup of this file, check it is not existing.
                // If it does, just verify overwriteCheckBox

                QString filePath = installationPathLabel->text() + QDir::separator() +
                                   sitesToBackup.at(i)->name() + QDir::separator() +
                                   sitesToBackup.at(i)->albums().at(j)->alias() + QDir::separator() +
                                   fileName;

                if( helloTipiBackupEngine::verifyFile( filePath ) == false
                || overwriteCheckBox->checkState() == Qt::Checked )
                {
                    currentQuery->setHost(pictureUrl.host());
                    id = currentQuery->get(pictureUrl.path());

                    qDebug() << "#" << id << " - Doing query on " << pictureUrl.host() << " / " << pictureUrl.path() << " - file is " << fileName;

                    numberFileTotal ++;
                    queries[id] << sitesToBackup.at(i)->name() << sitesToBackup.at(i)->albums().at(j)->alias() << fileName;
                }
            }
        }
    }

    qDebug() << "Nombre de fichiers à télécharger: " << numberFileTotal;

    if(numberFileTotal == 0)
    {
        isDownloadDone = true;

        currentFileLabel->setText(tr("Pas de fichier à télécharger. Terminé !"));

        progressBar->setValue(100);

        endDownloadButton->setText(tr("Quitter"));
    } else {
        startTime = QDateTime::currentDateTime();
    }

    return ;
}

void helloTipiBackupMainWindow::startBlockingQuery(QString url,
                                                   QHash<QString, QString> args)
{
    m_eventLoop = new QEventLoop(this);

    connect(this, SIGNAL(singleQueryDone()), m_eventLoop, SLOT(quit()));
    connect(currentQuery, SIGNAL(done(bool)),
            this, SLOT(blockingQueryFinished(bool)));

    /* Build query */
    QString argstr = QString("?");
    QHashIterator<QString, QString> i(args);
    while(i.hasNext()) {
        i.next();
        argstr += i.key() + QString("=") + i.value() + QString("&");
    }

    if(args.count() > 0)
        url += argstr;

    qDebug() << "Doing query: " << url;
    currentQuery->get(url);

    m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);

    disconnect(currentQuery, SIGNAL(done(bool)), 0, 0);
    disconnect(this, SIGNAL(singleQueryDone()), 0, 0);
}

void helloTipiBackupMainWindow::blockingQueryFinished(bool error)
{
    qDebug() << "Will emit signal for a code: " << currentQuery->lastResponse().statusCode();
    if(error)
    {
        qDebug() << "There is an error !";
    }
    emit singleQueryDone();
}


void helloTipiBackupMainWindow::downloadRequestStarted(int id)
{
    qDebug() << "Call of downloadRequestStarted with #" << id;

    if(queries.contains(id))
    {
        currentFileLabel->setText( tr("%1 (album : %2)")
                                 .arg(queries[id][2], queries[id][1]));
    }
}

void helloTipiBackupMainWindow::downloadRequestFinished(int id, bool error)
{
    qDebug() << "Call of downloadRequestFinished with #" << id << " - state is " << error;
    qDebug() << "Error " << currentQuery->error();

    QHttpResponseHeader header = currentQuery->lastResponse();

    if(header.isValid())
    {
        qDebug() << header.statusCode();
        qDebug() << header.reasonPhrase();
    }
    else
    {
        qDebug() << "Header is not valid.";
    }

    /* Flush Buffer */
    QByteArray datas = currentQuery->readAll();

    qDebug() << "Size of datas: " << datas.size();

    if(datas.size() == 0 || queries.contains(id) == false)
        return;

    qDebug() << "Opening file: " << queries[id].last();
    QString newFilePath = installationPathLabel->text() + QDir::separator() +
                          queries[id][0] + QDir::separator() +
                          queries[id][1] + QDir::separator() +
                          queries[id][2];

    QFile newFile(newFilePath);

    newFile.open(QIODevice::WriteOnly);
    newFile.write(datas);
    newFile.close();

    numberFileDone ++;

    progressBar->setValue((int)round(100 * (double)numberFileDone / (double)numberFileTotal));
}

void helloTipiBackupMainWindow::downloadDataReadProgress(int done, int total)
{
    qDebug() << "Call of downloadDataReadProgress with done: " << done << " - total: " << total;

    if(total)
    {
        float doneValue = round(100.0 * ((double)numberFileDone / (double)numberFileTotal + (1.0/(double)numberFileTotal) * (double)done/(double)total));
        int newValue = (int)doneValue;
        progressBar->setValue( newValue );

        int secsTo = startTime.secsTo( QDateTime::currentDateTime() );

        qDebug() << "Seconds : " << secsTo;

        int needToDo = round((100.0 * (double)secsTo / (double)doneValue) - (double)secsTo);
        int secs = needToDo % 60;
        int mins = ((needToDo - secs) / 60) % 60;
        int hours = (((needToDo - (mins * 60) - secs) / 60) / 60);

        QString eta = QString();

        if(hours)
            eta += QString(tr("%1 heure")).arg(hours);
        if(mins)
            eta += QString(tr("%1 min")).arg(mins);

        if(secs)
            eta += QString(tr("%1 s")).arg(secs);
        else
        {
            if(mins == 0 && hours == 0)
                eta += QString(tr("Presque fini !"));
        }

        currentFileCountLabel->setText(tr("Fichier %1 sur %2 / Temps restant estimé: %3").arg(numberFileDone + 1).arg(numberFileTotal).arg(eta) );
    }
}

void helloTipiBackupMainWindow::downloadDone(bool error)
{
    qDebug() << "Call of downloadDone with: " << error;

    isDownloadDone = true;

    if(error == true)
    {
        currentFileLabel->setText(tr("Annulé."));
    }
    else
    {
        currentFileLabel->setText(tr("Terminé. %1 fichier%2 téléchargé%2.").arg(numberFileDone)
                                                                           .arg((numberFileDone > 1)?QString("s"):QString()));
    }

    currentFileCountLabel->setText(QString());
    endDownloadButton->setText(tr("Quitter"));
}

void helloTipiBackupMainWindow::changeDirectoryButtonPushed(void)
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Choix du répertoire"),
                                                          QString(QDir::homePath()),
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if(directory != QString())
        installationPathLabel->setText(directory);

    return;
}

void helloTipiBackupMainWindow::endDownloadButtonPushed(void)
{
    int resp;

    if(isDownloadDone == false)
    {
        qDebug() << "Stopping the download";

        resp = QMessageBox::question(this, tr("Arret"), tr("Voulez vous arreter le téléchargement ?"), QMessageBox::Yes | QMessageBox::No);

        if(resp == QMessageBox::Yes)
        {
            isDownloadDone = true;

            currentQuery->abort();

            endDownloadButton->setText(tr("Quitter"));
        }
    }
    else
    {
        qDebug() << "Closing the application...";
        close();
    }

    return;
}

void helloTipiBackupMainWindow::aboutLabelPushed(QString url)
{
    qDebug() << "About Button pushed...";

    QMessageBox::information(this, tr("A propos d'helloTipiBackup"), tr("helloTipiBackup v1.0\nhttp://www.hellotipi.com/"));

    return ;
}

void helloTipiBackupMainWindow::readSettings(void)
{
    QSettings settings("helloTipi", "helloTipiBackup");

    QString user_login, user_password, user_path;
    bool overwrite_files, save_parameters;

    qDebug() << "readSettings";

    settings.beginGroup("main");
    user_login = settings.value("login").toString();
    user_password = settings.value("password").toString();
    user_path = settings.value("path").toString();
    overwrite_files = settings.value("overwrite").toBool();
    save_parameters = settings.value("save_login").toBool();
    settings.endGroup();

    if(user_login != QString())
        loginEdit->setText( user_login );

    if(user_password != QString())
        passwordEdit->setText( user_password );

    if(user_path != QString())
        installationPathLabel->setText( user_path );

    if( save_parameters )
        dontForgetCheckBox->setCheckState( Qt::Checked );

    if( overwrite_files )
        overwriteCheckBox->setCheckState( Qt::Checked );

    return ;
}

void helloTipiBackupMainWindow::writeSettings(void)
{
    QSettings settings("helloTipi", "helloTipiBackup");

    qDebug() << "writeSettings";

    settings.beginGroup("main");
    if(dontForgetCheckBox->checkState() == Qt::Checked)
    {
        settings.setValue("login", loginEdit->text());
        settings.setValue("password", passwordEdit->text());

        settings.setValue("save_login", true);
    }
    else
    {
        settings.setValue("login", QString());
        settings.setValue("password", QString());

        settings.setValue("save_login", false);
    }

    if(overwriteCheckBox->checkState() == Qt::Checked)
    {
        settings.setValue("overwrite", true);
    }
    else
    {
        settings.setValue("overwrite", false);
    }
    settings.setValue("path", installationPathLabel->text());

    settings.endGroup();
    return ;
}

void helloTipiBackupMainWindow::backToSiteButtonPushed(void)
{
    stackedWidget->setCurrentIndex( 1 );

    return;
}


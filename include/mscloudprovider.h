/*
    CloudCross: Opensource program for syncronization of local files and folders with clouds

    Copyright (C) 2016  Vladimir Kamensky
    Copyright (C) 2016  Master Soft LLC.
    All rights reserved.


  BSD License

  Redistribution and use in source and binary forms, with or without modification, are
  permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
  - Neither the name of the "Vladimir Kamensky" or "Master Soft LLC." nor the names of
    its contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY E
  XPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES O
  F MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SH
  ALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENT
  AL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROC
  UREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS I
  NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRI
  CT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF T
  HE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MSCLOUDPROVIDER_H
#define MSCLOUDPROVIDER_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileInfoList>
#include <QTextStream>
#include <QDataStream>
#include <QCryptographicHash>
#include <QList>
#include <QRegularExpression>
#include <QNetworkProxy>

#include <QTcpServer>

#include "msfsobject.h"
#include "msidslist.h"
#include "msrequest.h"
#include "qstdout.h"




class MSCloudProvider : public QObject
{


    Q_OBJECT

private:




public:

    QNetworkProxy* proxyServer;

    QString currentPath;// directory where program was run
    QString workPath;// path set with -p option

    enum CloudObjects{
                        Files=1,
                        Folders=2,
                        FilesAndFolders=3,
                        NoTrash=4
    };

    enum SyncStrategy{
                        PreferLocal=1,
                        PreferRemote=2
    };

    MSCloudProvider(QObject* parent=0);

    QString providerName;

    QString tokenFileName;
    QString stateFileName;
    QString trashFileName;

    QString token;
    QString access_token;

    qint64 lastSyncTime;// date and time of last sync

    SyncStrategy strategy; // sync strategy


    MSIDsList ids_list;


    bool setProxyServer(QString type, QString proxy);

    //Filters
    bool filterServiceFileNames(const QString &path);
    bool filterIncludeFileNames(const QString &path);
    bool filterExcludeFileNames(const QString &path);


    virtual bool auth() = 0;
    virtual void saveTokenFile(QString path) =0 ;
    virtual bool loadTokenFile(QString path)=0;
    virtual void loadStateFile()=0;
    virtual void saveStateFile()=0;
    virtual bool refreshToken()=0;
    virtual MSFSObject::ObjectState filelist_defineObjectState(MSLocalFSObject local, MSRemoteFSObject remote)=0;
    virtual void doSync()=0;
    virtual bool remote_file_generateIDs(int count) =0 ;

    virtual QHash<QString,MSFSObject>   filelist_getFSObjectsByState(MSFSObject::ObjectState state) =0;
    virtual QHash<QString,MSFSObject>   filelist_getFSObjectsByState(QHash<QString,MSFSObject> fsObjectList,MSFSObject::ObjectState state) =0;
    virtual QHash<QString,MSFSObject>   filelist_getFSObjectsByTypeLocal(MSLocalFSObject::Type type)=0;
    virtual QHash<QString,MSFSObject>   filelist_getFSObjectsByTypeRemote(MSRemoteFSObject::Type type)=0;
    virtual bool                        filelist_FSObjectHasParent(MSFSObject fsObject)=0;
    virtual MSFSObject                  filelist_getParentFSObject(MSFSObject fsObject)=0;
    virtual void                        filelist_populateChanges(MSFSObject changedFSObject)=0;
//    virtual void                        filelist_populateChanges(QHash<QString,MSFSObject> changedFSObjectList)=0;


    bool local_writeFileContent(QString filePath, MSRequest *req);


    void local_createDirectory(QString path);
    virtual void local_removeFile(QString path) =0;
    virtual void local_removeFolder(QString path) =0;
    //void local_createDirectory(QString path);


    QString fileChecksum(QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);

    QString generateRandom(int count);

    // converts to milliseconds in UTC timezone
    qint64 toMilliseconds(QDateTime dateTime, bool isUTC=false);
    qint64 toMilliseconds(QString dateTimeString, bool isUTC=false);


    // Modify Mode Flags

    bool useInclude;
    bool dryRun;
    bool noHidden;
    bool newRev; //???????

    QHash<QString,bool>flags;
    QHash<QString,QString>options;

    QString includeList;
    QString excludeList;

    void setFlag(QString flagName, bool flagVal);
    bool getFlag(QString flagName);

    QString getOption(QString optionName);

    virtual bool testReplyBodyForError(QString body) = 0;
    virtual QString getReplyErrorString(QString body) = 0;

    virtual bool directUpload(QString url,QString remotePath) =0;




    QTcpServer* oauthListener;
    QTcpSocket* oauthListenerSocket;

    bool startListener(int port);
    bool stopListener();

    bool providerAuthStatus;

public slots:

    void onIncomingConnection();
    void onDataRecieved();

    //void onAuthComplete(QString code);


    virtual bool onAuthFinished(QString html, MSCloudProvider *provider)=0;

signals:

    void oAuthCodeRecived(QString code,MSCloudProvider* provider);
    void oAuthError(QString code,MSCloudProvider* provider);


    void providerAuthComplete();

};


#endif // MSCLOUDPROVIDER_H

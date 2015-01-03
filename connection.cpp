#include "connection.h"
#include "usermanager.h"
#include <QDebug>

const unsigned long Connection::SESSION_TIMER = 5;

Connection::Connection(QTcpSocket* client) : QThread()
{
    this->client = client;
    this->blockSignals( false );
}

void Connection::run()
{

    bool loginCompleted = false;
    QString user;
    QString answer;
    do
    {
        do
        {
            this->client->write( "who?\n");

            this->client->waitForReadyRead( -1 );
            user = this->client->readLine();
            user.chop( 2 );
            qDebug()<< user;

        }while( !UserManager::getInstance()->isAUser( user ) );


        this->client->write( "ok pls pw\n");

        this->client->waitForReadyRead( -1 );
        answer = this->client->readLine();
        answer.chop( 2 );

        int attempt = 0;
        while( !(loginCompleted = UserManager::getInstance()->getUser( user )->isPasswdCorrect( answer ) ) && attempt < 3 )
        {
            this->client->write( "try again\n" );
            this->client->waitForReadyRead( -1 );
            answer = this->client->readLine();
            attempt++;
        }
    }while( !loginCompleted );

    this->client->write( "ok\n" );



    QString message("FileTransfer=80001 ");
    message += "DLNA=80002 ";
    message += "STREAMING=80003\n";
    this->client->write( message.toUtf8() );
    this->client->waitForBytesWritten( -1 );

    while( true )
    {
        sleep( SESSION_TIMER );

        this->client->write( "are you there?\n" );
        this->client->waitForBytesWritten( -1 );

        bool hasItAnswered = this->client->waitForReadyRead( 2000 );

        if( hasItAnswered )
            answer = this->client->readLine();
        else
        {
            UserManager::getInstance()->getUser( user )->setIsOnline( false );
            UserManager::getInstance()->getUser( user )->setSessionID( -1 );
            this->client->close();

            emit closed( this );
            break;
        }

    }
    this->terminate();
}

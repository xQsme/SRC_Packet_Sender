#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTime>
#include <QtDebug>
#include <QFile>

void help();
void chooseFile(QString file);
void interval(int millisecondsToWait);
void total(int num);
void delay( int millisecondsToWait );

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
        QCommandLineParser parser;
        QCommandLineOption helpOption(QStringList() << "h" << "help",
                    QCoreApplication::translate("main", "Usage Help"));
        parser.addOption(helpOption);
        QCommandLineOption targetInputOption(QStringList() << "f" << "file",
                    QCoreApplication::translate("main", "Specify a file"));
        parser.addOption(targetInputOption);
        QCommandLineOption intervalOption(QStringList() << "i" << "interval" ,
                    QCoreApplication::translate("main", "chosse a interval in ms"));
        parser.addOption(intervalOption);
        QCommandLineOption totalOption(QStringList() << "t" << "total",
                    QCoreApplication::translate("main", "amount of time to send package"));
        parser.addOption(totalOption);

        parser.process(a);

        QStringList args = parser.optionNames();

        if(args.contains("h") || args.contains("help")){
            help();
            return 0;
        }

        if(args.contains("f") || args.contains("file")){
            QString file;
            file=parser.value(targetInputOption);
            chooseFile(file);

        }

        if(args.contains("i") || args.contains("interval")){
           int ms =  parser.value(intervalOption).toInt();
           interval(ms);
        }

        if(args.contains("t") || args.contains("total")){
            int num =  parser.value(totalOption).toInt();
            total(num);
        }



    return 0;
}

void help()
{
    qDebug() << "Usage:\n-f   --file\t\t\tChoose a file to send" <<
                "\n-i   --interval\t\t\tDInterval to send package in ms" <<
                "\n-t   --total\t\t\tamount of time to send the same package" ;
}

void chooseFile(QString file){
    qDebug() << "file";

    if (file.isEmpty()){
        qDebug() << "Choose a file";
    }else{
        QFile ficheiro(file);

        if (!ficheiro.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Not possible to open file";
            return;
        }else{
            qDebug() << "File open";
            QString content = ficheiro.readAll();
            ficheiro.close();
        }
    }
}

void interval(int millisecondsToWait){
    qDebug() << "invervalo";
    delay(millisecondsToWait);
    qDebug() << "invervalo2";

}

void delay( int millisecondsToWait )
{
    QTime *dieTime = new QTime(QTime::currentTime().addMSecs( millisecondsToWait ));
    while( QTime::currentTime() < *dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void total(int num){

    /*for(int i=0;i<num;i++){
        //send package
    }*/
   qDebug() << "total";
}

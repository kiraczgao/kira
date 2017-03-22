#include "myposwidget.h"
#include <unistd.h>
#include <QApplication>
#include <QStackedLayout>
#include <stdio.h>

#define PIDFILE "/opt/kira/pidfile.txt"
#define POS_VERSION "V3.0.2_20161126-test \n"

int main(int argc, char *argv[])
{
    printf("POS Version: %s \n", POS_VERSION);
    if(argc == 2)
    {
        if(*argv[1] == 'v')
        {
            printf(POS_VERSION);
            return 1;
        }
    }

    QApplication a(argc, argv);
    myPosWidget w;

    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setCursor(Qt::BlankCursor);
    w.showMaximized();

    FILE *fp;
    pid_t pidno = getpid();
    printf("pid no = %d\n", pidno);
    fp = fopen(PIDFILE, "wb+");
    fwrite(&pidno, 4, 1, fp);
    fclose(fp);

    return a.exec();
}

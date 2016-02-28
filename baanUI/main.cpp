#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QCommandLineOption>
#include <QCommandLineParser>
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationVersion("1.0");
    //QLocale::setDefault(QLocale::c());
    //QLocale loc;
    //printf("locale %c\n",loc.decimalPoint().toLatin1());

    QCommandLineParser parser;
    parser.setApplicationDescription("Baan program");

    // Standard -h / --help options.
    parser.addHelpOption();

    // Standard -v / --version options.
    parser.addVersionOption();

    // A boolean option edit  (-e)
    QCommandLineOption editOption("e", QCoreApplication::translate("main", "Run program in edit mode"));
    parser.addOption(editOption);

    // Accepts a single filename argument.
    parser.addPositionalArgument("filename", QApplication::translate("main", "blk file om te openen"));


   parser.process(application);

    // kijk of er een filename gegeven is
    const QStringList args = parser.positionalArguments();
    std::string inputFilename;
    if (args.size() == 1)
    {
        inputFilename = args[0].toStdString();
    }

    MainWindow w(nullptr, inputFilename, parser.isSet(editOption));
    w.show();

    return application.exec();
}

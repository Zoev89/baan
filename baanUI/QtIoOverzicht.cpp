#include "QtIoOverzicht.h"
#include "ui_QtIoOverzicht.h"
#include <sstream>

QtIoOverzicht::QtIoOverzicht(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtIoOverzicht)
{
    ui->setupUi(this);
}

QtIoOverzicht::~QtIoOverzicht()
{
    delete ui;
}

void QtIoOverzicht::Display(const std::vector<std::string>& list)
{
    ui->mTable->setRowCount(list.size()>0 ? list.size()-1: 0);
    ui->mTable->setColumnCount(17);
    for (int i=0;i<17;++i)
    {
        ui->mTable->setColumnWidth(i, 45);
    }
    for(size_t index=0; index<list.size();++index)
    {
        std::string buf; // Have a buffer string
        std::stringstream ss(list[index]); // Insert the string into a stream

        std::vector<std::string> tokens; // Create vector to hold our words

        while (ss >> buf)
        {
            tokens.push_back(buf);
        }
        if (index == 0)
        {
            QStringList horzHeaders;
            for(size_t i=0;i<tokens.size(); ++i)
            {
                horzHeaders << tokens[i].c_str();
            }
            ui->mTable->setHorizontalHeaderLabels( horzHeaders );
        }
        else
        {
            for(size_t i=0;i<tokens.size(); ++i)
            {
                auto item = new QTableWidgetItem(tokens[i].c_str());
                ui->mTable->setItem(index-1,i,item);
            }

        }
    }
    show();
}

void QtIoOverzicht::closeDialog()
{
    hide();
}

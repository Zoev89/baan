#include "QtBaanOverzicht.h"
#include "ui_QtBaanOverzicht.h"
#include <boost/tokenizer.hpp>

QtBaanOverzicht::QtBaanOverzicht(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtBaanOverzicht)
{
    ui->setupUi(this);
}

QtBaanOverzicht::~QtBaanOverzicht()
{
    delete ui;
}

void QtBaanOverzicht::Display(const std::vector<std::string>& list)
{
    ui->mTable->setRowCount(list.size());
    ui->mTable->setColumnCount(2);
    ui->mTable->setColumnWidth(0, 100);
    ui->mTable->setColumnWidth(1, 200);
    QStringList horzHeaders;
    horzHeaders << "id";
    horzHeaders << "soort";
    ui->mTable->setHorizontalHeaderLabels( horzHeaders );
    for(size_t index=0; index<list.size();++index)
    {
        boost::char_separator<char> sep("\t");
        boost::tokenizer<boost::char_separator<char>> tokens(list[index], sep);

            int i=0;
            for(const auto& t:tokens)
            {
                auto item = new QTableWidgetItem(t.c_str());
                ui->mTable->setItem(index,i++,item);
            }

    }
    show();
}

void QtBaanOverzicht::closeDialog()
{
    hide();
}

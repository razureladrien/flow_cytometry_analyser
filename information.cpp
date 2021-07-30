#include "information.h"
#include "ui_information.h"
#include <QDebug>

InformationDialog::InformationDialog(QWidget *parent, QString file, int events, int params, QList<QString> parameters_list) :
    QDialog(parent),
    ui(new Ui::InformationDialog)
{
    setWindowTitle("Informations");
    QString parameters = "";
    for (auto i: parameters_list)
        parameters += i+" | ";
    ui->setupUi(this);
    ui->textEdit_file->setReadOnly(true);
    ui->textEdit_file->setText(file);
    ui->textEdit_data_dim->setReadOnly(true);
    ui->textEdit_data_dim->setText(QString::number(events) + " (events), " + QString::number(params) + " (parameters)");
    ui->textEdit_parameters->setReadOnly(true);
    ui->textEdit_parameters->setText(parameters);
}

InformationDialog::~InformationDialog()
{
    delete ui;
}

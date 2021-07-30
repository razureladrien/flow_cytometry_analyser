#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent, double size) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    ui->doubleSpinBox->setDecimals(1);
    ui->doubleSpinBox->setSingleStep(0.1);
    ui->doubleSpinBox->setValue(size);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_buttonBox_accepted()
{
    emit marker_size(ui->doubleSpinBox->value());
}

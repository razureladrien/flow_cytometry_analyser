#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr, double size = 1);
    ~Settings();

private slots:
    void on_buttonBox_accepted();

signals:
    void marker_size(double);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H

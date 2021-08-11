#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

/*
 * UI that displays a setting window
 * For now you can change the size of the dots in the plots
 */
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

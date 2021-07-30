#ifndef INFORMATION_H
#define INFORMATION_H

#include <QDialog>

namespace Ui {
class InformationDialog;
}

class InformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InformationDialog(QWidget *parent = nullptr, QString file = "", int events = 0, int params = 0, QList<QString> parameters_list = {});
    ~InformationDialog();

private:
    Ui::InformationDialog *ui;
};

#endif // INFORMATION_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "sqllitutil.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btncreteTbl_clicked();

    void on_btnCreateData_clicked();

private:
    Ui::Widget *ui;

    sqlLitUtil *sqllitUtil;
};

#endif // WIDGET_H

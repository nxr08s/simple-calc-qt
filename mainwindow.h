#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class StateMachine;

class MainWindow : public QMainWindow{
    Q_OBJECT

    bool isDisabled;

    StateMachine* machine;
    void controlsEnable(bool);

    void keyPressEvent(QKeyEvent* e) override;
    void processInput(QChar);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void controlClicked();
    void resultClicked();
    void clearClicked();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QMessageBox>
#include <QKeyEvent>

#include "statemachine.h"

// обработчик клавиш(дублирует кнопки интерфейса)
void MainWindow::keyPressEvent(QKeyEvent *e){
    QChar ch;
    if (e->key() >= Qt::Key_0 && e->key() <= Qt::Key_9)
        ch = '0' + (e->key() - Qt::Key_0);
    switch(e->key()){
    case Qt::Key_Asterisk:
        ch = '*';
        break;
    case Qt::Key_Slash:
        ch = '/';
        break;
    case Qt::Key_Return:
        resultClicked();
        return;
    case Qt::Key_Escape:
        clearClicked();
        return;
    case Qt::Key_Plus:
        ch = '+';
        break;
    case Qt::Key_Minus:
        ch = '-';
        break;
    case Qt::Key_ParenLeft:
        ch = '(';
        break;
    case Qt::Key_ParenRight:
        ch = ')';
        break;
    case Qt::Key_Period:
        ch = '.';
        break;
    }
    processInput(ch);
    QWidget::keyPressEvent(e);
}

// отправляет полученный символ автомату, и если выражение корректно, введеный символ добавляется на форму
void MainWindow::processInput(QChar ch){
    if (!isDisabled){
        if (machine->parseChar(ch.unicode()))
            ui->lineEdit->setText(ui->lineEdit->text() + ch);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    machine(new StateMachine),
    isDisabled(false)
{
    ui->setupUi(this);

    // соединение клавиш операторов
    connect(ui->minusBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->plusBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->mulBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->divBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->openBrBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->closeBrBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));

    // соединение клавиш цифр
    connect(ui->zeroBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->oneBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->twoBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->threeBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->fourBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->fiveBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->sixBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->sevenBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->eightBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));
    connect(ui->nineBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));

    // соединение десятичной точки
    connect(ui->pointBtn, SIGNAL(clicked(bool)), this, SLOT(controlClicked()));

    // соединение клавиши очистить
    connect(ui->clearBtn, SIGNAL(clicked(bool)), this, SLOT(clearClicked()));

    // соединение клавиши равно
    connect(ui->resultBtn, SIGNAL(clicked(bool)), this, SLOT(resultClicked()));
}

MainWindow::~MainWindow(){
    delete ui;
    delete machine;
}

// все кнопки соединены с этим слотом, необходимо узнать какая нажата и отправить автомату
void MainWindow::controlClicked(){
    QPushButton* btn = (QPushButton*)sender();
    QChar ch = btn->text().at(0);
    processInput(ch);
}

// обработчик нажатия на enter или "=" на форме
void MainWindow::resultClicked(){
    if (isDisabled)
        return;

    if (!machine->finalParseStep()) // финальная проверка строки (соотвествие скобок и тд)
        return;

    // само вычисление
    if(machine->calculate())
        ui->lineEdit->setText(ui->lineEdit->text() + QChar('=') + QString::number(machine->result()));
    else
        QMessageBox::critical(nullptr, "Ошибка", "Произошло деление на 0");
    isDisabled = true;  // результат есть (или нет), ничего нажимать больше нельзя, только сбросить (или esc)
}

// обработчик сброса (или клавиши esc)
void MainWindow::clearClicked(){
    machine->reset();
    ui->lineEdit->clear();
    isDisabled = false;
}

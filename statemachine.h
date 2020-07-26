/*
 *  Таблица переходов для "валидатора" исходной строки (она же и делит ее на токены):
 *  Легенда:    err - ошибка
 *              opr - в бинарном операторе (оно же и начальное, и про входе в скобки)
 *              num - в числе (до десятичной точки)
 *              pnt - в десятичной точке
 *              apt - после десятичной точки в числе
 *              unr - унарный минус
 *              clb - в закрывабщейся скобке
 *
 *       ____________________________________________________________________________
 *      |    State   |   +   |   -   |   *   |   /   |   .   |   (   |   )   |   num |
 *      |____________|_______|_______|_______|_______|_______|_______|_______|_______|
 *      |    opr     |   err |   unr |   err |   err |   err |   opr |   err |   num |
 *      |    num     |   opr |   opr |   opr |   opr |   pnt |   err |   clb |   num |
 *      |    pnt     |   err |   err |   err |   err |   err |   err |   err |   apt |
 *      |    apt     |   opr |   opr |   opr |   opr |   err |   err |   clb |   apt |
 *      |    unr     |   err |   err |   err |   err |   err |   opr |   err |   num |
 *      |    clb     |   opr |   opr |   opr |   opr |   err |   err |   clb |   err |
 *      |____________|_______|_______|_______|_______|_______|_______|_______|_______|
 *
 * */
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QString>
#include <QVector>
#include <QStack>

class StateMachine{
    // состояния вышеуказанной таблицы переходов
    enum class State{
        Error,
        InOperator,
        InNumber,
        InDecPoint,
        AfterDecPoint,
        InUnaryMinus,
        InCloseBr
    };
    QString strNum; // временное хранение чисел в строковом виде
    State prevState;
    State currentState;
    State getState(const char ch);

    enum class TokenType{ Operator, Number };
    struct Token{
        TokenType type;
        double number;
        char op;
        Token(TokenType t, double n):type(t), number(n), op('\0'){}
        Token(TokenType t, char o):type(t), op(o), number(0){}
    };
    QVector<Token*> tokens;
    QStack<double> numStack;
    QStack<char> opStack;


    unsigned int bracketsCount;

    static unsigned int priority(const char ch);
    bool execute();


public:
    StateMachine();

    bool parseChar(const char ch);
    bool finalParseStep();
    bool calculate();
    void reset();
    double result() const;
};

#endif // STATEMACHINE_H

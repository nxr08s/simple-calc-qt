#include "statemachine.h"

StateMachine::StateMachine() : currentState(State::InOperator), bracketsCount(0) {}

unsigned int StateMachine::priority(const char ch){
    if (ch == '-' || ch == '+')
        return 1;
    if (ch == '/' || ch == '*')
        return 2;
    if (ch == '~')
        return 3;
    return INT_MAX;
}

// функция пердставляет из себя "таблицу" переходов, изображенную в заг. файле
//  лишь меняет состояние и проверяет входную строку, с данными ничего не делает
StateMachine::State StateMachine::getState(const char ch){
    switch(currentState){
    case State::InOperator:
        if (QChar::isDigit(ch)) return State::InNumber;
        switch(ch){
        case '-':   return State::InUnaryMinus;
        case '(':   return State::InOperator;
        }
        break;
    case State::InNumber:
        if (QChar::isDigit(ch)) return State::InNumber;
        switch(ch){
        case '+':
        case '-':
        case '*':
        case '/':   return State::InOperator;
        case '.':   return State::InDecPoint;
        case ')':   return State::InCloseBr;
        }
        break;
    case State::InDecPoint:
        if (QChar::isDigit(ch))
            return State::AfterDecPoint;
        break;
    case State::AfterDecPoint:
        if (QChar::isDigit(ch)) return State::AfterDecPoint;
        switch(ch){
        case '+':
        case '-':
        case '*':
        case '/':   return State::InOperator;
        case ')':   return State::InCloseBr;
        }
        break;
    case State::InUnaryMinus:
        if (QChar::isDigit(ch))
            return State::InNumber;
        else if (ch == '(')
            return State::InOperator;
        break;
    case State::InCloseBr:
        switch(ch){
        case '-':
        case '+':
        case '*':
        case '/':   return State::InOperator;
        case ')':   return State::InCloseBr;
        }
    }
    return State::Error;
}

// сама функция "захвата" символов. Если переход в состояние прошел успешно, обрабатывается символ
//  и, в процессе, решается что это: число или оператор
bool StateMachine::parseChar(const char ch){
    State st = getState(ch);

    // если закрывающаяся скобка без открывающейся, то просто игнорируем ввод этого символа
    if ((st == State::Error) || (ch == ')' && bracketsCount == 0))
        return false;

    // смена состояния автомата
    prevState = currentState;
    currentState = st;

    if (ch == '(')
        bracketsCount++;
    if (ch == ')')
        bracketsCount--;

    switch (st) {
    case State::InNumber:
    case State::InDecPoint:
    case State::AfterDecPoint:  // захват цифры числа
        strNum.push_back(ch);
        break;
    case State::InCloseBr:
    case State::InOperator: // если предыдущее состояние было в числе, то это число необходимо перевести
                            // в double и закинуть в вектор с токенами
        if (prevState == State::InNumber || prevState == State::AfterDecPoint){
            tokens.append(new Token(TokenType::Number, strNum.toDouble()));
            strNum.clear();
        }
        tokens.append(new Token(TokenType::Operator, ch));  // и добавить в вектор сам оператор
        break;
    case State::InUnaryMinus:   // обработка унарного минуса происходит посредством добавления 0 и
                                // импровизированного оператора "унарный минус(~)". Отличается от обычного
                                // минуса наибольшим приоритетом (самый высоких из всех возможных)
                                // т.о. при разборе выражения, это вычитание произведется сразу же как попадется
        tokens.append(new Token(TokenType::Number, 0.0f));
        tokens.append(new Token(TokenType::Operator, '~'));
        break;
    }
    return true;
}

// проверка скобок, и добавление последнего числа (которое будучи в самом конце, не захватывается в ходе разбиения)
bool StateMachine::finalParseStep(){
    if (bracketsCount != 0)
        return false;
    // если выражение заканчивается в допустимом состоянии(нет пустых операторов и тд)
    if (currentState == State::AfterDecPoint || currentState == State::InCloseBr || currentState == State::InNumber){
        if (!strNum.isEmpty()){
            tokens.append(new Token(TokenType::Number, strNum.toDouble()));
            strNum.clear();
        }
        return true;
    }
    return false;
}

// решение выражения, разбитого на токены. На этом этапе, гарантируется корректность выражения,
// поэтому единственная возможная ошибка - деление на 0. При обнаружении ее, разбор будет завершен и вверх по
// стеку вызовов вернется false и отборазится окно с ошибкой
bool StateMachine::calculate()
{
    for (Token* token : tokens) {
        if (token->type == TokenType::Number)   // число просто дабвляем в стек числе
            numStack.push(token->number);
        else {
            switch (token->op) {
            case '(':
            case '~':
                opStack.push(token->op);
                break;;
            case ')':
                // при обнаружении ')', выполняются все операции из стека операций до ближайшей '('
                while (opStack.top() != '(')
                    if(!execute())
                        return false;
                opStack.pop();
                break;
            default:
                // выполнение всех операций с более высоким приоритетом, или пока не попадется '(', или пока операции в
                // стеке не закончатся
                if (!opStack.empty() && priority(token->op) <= priority(opStack.top()))
                    while (!opStack.empty() && priority(token->op) <= priority(opStack.top()) && opStack.top() != '(')
                        if(!execute())
                            return false;
                opStack.push(token->op);
            }
        }
    }
    // выполенние всех оставшихся в стеке операций
    while (!opStack.empty())
        if (!execute())
            return false;

    return true;
}

// выполнение операции: из стека чисел берутся 2 верхних числа, из стека операторов так же берется высший оператор,
// и затем в стек числе помещается результат операции
bool StateMachine::execute(){
    double value1, value2, returnValue;

    value2 = numStack.top();
    numStack.pop();

    value1 = numStack.top();
    numStack.pop();

    switch (opStack.top()) {
    case '-':
    case '~':
        returnValue = value1 - value2;
        break;
    case '+':
        returnValue = value1 + value2;
        break;
    case '*':
        returnValue = value1 * value2;
        break;
    case '/':
        if (!value2)
            return false;
        returnValue = value1 / value2;
    }

    opStack.pop();
    numStack.push(returnValue);

    return true;
}

// т.к гарантируется корректность выражения,в процессе вычислений не может быть ошибок(кроме /0),
// т.к, в конце расчета, в стеке чисел гарантированно будет 1 значение
double StateMachine::result() const{
    return numStack.top();
}

void StateMachine::reset(){
    // каскадное удаление всех токенов
    for (Token* tk : tokens)
        delete tk;
    tokens.clear();
    currentState = State::InOperator;
    bracketsCount = 0;

    numStack.clear();
    opStack.clear();
}

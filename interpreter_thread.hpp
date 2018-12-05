#include <QCoreApplication>
#include <QThread>
#include <QString>
#include <QDebug>

#include "interpreter.hpp"
#include "semantic_error.hpp"

class InterpreterThread: public QThread
{
    Q_OBJECT
    
public:
    
    struct Message {
        Expression expression;
        bool isError;  // Whether there was an error or not
        std::string errorMsg;
    };
    
    InterpreterThread(QObject * parent = 0);
    
    void runInterpreter(MessageQueue<std::string> & inputQueue, MessageQueue<Message> & outputQueue, std::atomic_bool & runInterpreter, Interpreter * interp);
    
    void startInterpreter();
    
    void stopInterpreter();
    
    void resetInterpreter();
    
    void run() { // started is emitted right before this runs
        
        QString result;
        QTextStream output(&result);
        
        output << "Hello from MyThread with id = " << QThread::currentThreadId();
        
        // emit done(result);
        
        // could go on and do other stuff
        // when this returns, finished is emitted
    }
    
private:
    
    Interpreter interp;
    
signals:
    
    void doneInterpreting(const QString &s);
};

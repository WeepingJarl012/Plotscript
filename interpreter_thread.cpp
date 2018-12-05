#include "interpreter_thread.hpp"

InterpreterThread::InterpreterThread(QObject * parent){
    if(parent!=nullptr){
        // Use variable, useless
    }
}

void InterpreterThread::runInterpreter(MessageQueue<std::string> & inputQueue, MessageQueue<Message> & outputQueue, std::atomic_bool & runInterpreter, Interpreter * interp){
    
    Message outputMsg;
    
    while (runInterpreter){
        
        std::string line;
        if (inputQueue.try_pop(line)){
            
            std::istringstream expression(line);
            
            
            if(!(*interp).parseStream(expression)){
                outputMsg.isError = true;
                outputMsg.errorMsg = "Invalid Expression. Could not parse.";
                outputQueue.push(outputMsg);
            }
            else{
                try{
                    Expression exp = (*interp).evaluate(); // Output created
                    outputMsg.isError = false;
                    outputMsg.expression = exp;
                    outputQueue.push(outputMsg);
                }
                catch(const SemanticError & ex){
                    outputMsg.isError = true;
                    outputQueue.push(outputMsg);
                    std::cerr << ex.what() << std::endl;
                }
            }
        }
    }
}

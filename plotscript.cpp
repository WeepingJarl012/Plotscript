#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

struct Message {
    Expression expression;
    bool isError;  // Whether there was an error or not
    std::string errorMsg;
};

void prompt(){
    std::cout << "\nplotscript> ";
}

std::string readline(){
    std::string line;
    std::getline(std::cin, line);
    
    return line;
}

void error(const std::string & err_str){
    std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
    std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream){
    
    Interpreter interp;
    
    std::ifstream startup_stream(STARTUP_FILE);
    
    if(!startup_stream){
        error("Could not open startup file for reading.");
    }
    
    if(!interp.parseStream(startup_stream)){
        error("Invalid Program. Could not parse start up file.");
    }
    else{
        try{
            Expression exp = interp.evaluate();
        }
        catch(const SemanticError & ex){
            std::cerr << ex.what() << std::endl;
        }
    }
    
    if(!interp.parseStream(stream)){
        error("Invalid Program. Could not parse.");
        return EXIT_FAILURE;
    }
    else{
        try{
            Expression exp = interp.evaluate();
            std::cout << exp << std::endl;
        }
        catch(const SemanticError & ex){
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}

int eval_from_file(std::string filename){
    
    std::ifstream ifs(filename);
    
    if(!ifs){
        error("Could not open file for reading.");
        return EXIT_FAILURE;
    }
    
    return eval_from_stream(ifs);
}

int eval_from_command(std::string argexp){
    
    std::istringstream expression(argexp);
    
    return eval_from_stream(expression);
}

void interpret(MessageQueue<std::string> & inputQueue, MessageQueue<Message> & outputQueue){
    
    Interpreter interp;
    
    while (true){
        
        Message outputMsg;
        
        std::string line;
        if (inputQueue.try_pop(line)){
        
            std::istringstream expression(line);
            
            
            if(!interp.parseStream(expression)){
                outputMsg.isError = true;
                outputMsg.errorMsg = "Invalid Expression. Could not parse.";
                outputQueue.push(outputMsg);
                // error("Invalid Expression. Could not parse.");
            }
            else{
                try{
                    Expression exp = interp.evaluate(); // Output created
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

// A REPL is a repeated read-eval-print loop
void repl(){
    
    MessageQueue<std::string> inputQueue;
    MessageQueue<Message> outputQueue;
    
    std::thread interpretThread(interpret, std::ref(inputQueue), std::ref(outputQueue));
    
    std::ifstream startup_stream(STARTUP_FILE);
    
    if(!startup_stream){
        error("Could not open startup file for reading.");
    }
    
    Message outputMsg;
    
    while(!std::cin.eof()){
        
        prompt();
        std::string line = readline();
        
        if(line.empty()) continue;
        
        inputQueue.push(line);
        
        outputQueue.wait_and_pop(outputMsg);
        
        if (!outputMsg.isError){
            Expression exp = outputMsg.expression;
            
            std::cout << exp << std::endl;
        } else {
            outputMsg.isError = false;
            if (outputMsg.errorMsg != ""){
                error(outputMsg.errorMsg);
            }
        }
    }
    
    interpretThread.join();
}

int main(int argc, char *argv[])
{  
    if(argc == 2){
        return eval_from_file(argv[1]);
    }
    else if(argc == 3){
        if(std::string(argv[1]) == "-e"){
            return eval_from_command(argv[2]);
        }
        else{
            error("Incorrect number of command line arguments.");
        }
    }
    else{
        repl();
    }
    
    return EXIT_SUCCESS;
}

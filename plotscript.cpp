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

void interpret(MessageQueue<std::string> & inputQueue, MessageQueue<Expression> & outputQueue){
    Interpreter interp;
    
    std::string line;
    while (inputQueue.try_pop(line)){
    
        std::istringstream expression(line);
        
        
        if(!interp.parseStream(expression)){
            error("Invalid Expression. Could not parse.");
        }
        else{
            try{
                Expression exp = interp.evaluate(); // Output created
                outputQueue.push(exp);
            }
            catch(const SemanticError & ex){
                std::cerr << ex.what() << std::endl;
            }
        }
    }
    
    
}

// A REPL is a repeated read-eval-print loop
void repl(){
    
    MessageQueue<std::string> inputQueue;
    MessageQueue<Expression> outputQueue;
    
    std::thread interpretThread(interpret, std::ref(inputQueue), std::ref(outputQueue));
    
    std::ifstream startup_stream(STARTUP_FILE);
    
    if(!startup_stream){
        error("Could not open startup file for reading.");
    }
    
    /*
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
     */
    
    while(!std::cin.eof()){
        
        Expression exp;
        
        while (outputQueue.try_pop(exp)){
            std::cout << exp << std::endl;
        }
        
        prompt();
        std::string line = readline();
        
        // if(line.empty()) continue;
        
        inputQueue.push(line);
        
        /*
        std::istringstream expression(line);
        
        
        if(!interp.parseStream(expression)){
            error("Invalid Expression. Could not parse.");
        }
        else{
            try{
                Expression exp = interp.evaluate(); // Output created
                std::cout << exp << std::endl;
            }
            catch(const SemanticError & ex){
                std::cerr << ex.what() << std::endl;
            }
        }
         */
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

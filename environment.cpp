#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
 Helper Functions
 **********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
    return args.size() == nargs;
}

/*********************************************************************** 
 Each of the functions below have the signature that corresponds to the
 typedef'd Procedure function pointer.
 **********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
    args.size(); // make compiler happy we used this parameter
    return Expression();
};

Expression add(const std::vector<Expression> & args){
    
    bool complexArg = false;
    
    // check all aruments are numbers, while adding
    double realNumber = 0;
    double imagNumber = 0;
    
    for( auto & a :args){
        if(a.isHeadNumber()){
            realNumber += a.head().asNumber();
        } else if (a.isHeadComplex()){
            complexArg = true;
            
            realNumber += a.head().asComplex().real();
            imagNumber += a.head().asComplex().imag();
        }
        else{
            throw SemanticError("Error in call to add, argument not a number");
        }
    }
    
    if(complexArg){
        return Expression(std::complex<double>(realNumber, imagNumber));
    } else {
        return Expression(realNumber);
    }
};

Expression mul(const std::vector<Expression> & args){
    
    bool complexArg = false;
    bool firstPass = true;
    
    // check all aruments are numbers, while multiplying
    std::complex<double> complexResult (0, 0);
    
    for( auto & a :args){
        if(firstPass){
            firstPass = false;
            if (a.isHeadNumber()) {
                complexResult.real(a.head().asNumber());
            } else {
                complexArg = true;
                complexResult = a.head().asComplex();
            }
            
        } else if(a.isHeadNumber() && !complexArg){
            double result = a.head().asNumber() * complexResult.real();
            complexResult.real(result);
        } else if (a.isHeadComplex()) {
            complexArg = true;
            complexResult *= a.head().asComplex();
        } else if (a.isHeadNumber()) {
            complexResult *= a.head().asNumber();
        }
        else{
            throw SemanticError("Error in call to mul, argument not a number");
        }
    }
    
    if(complexArg){
        return Expression(complexResult);
    } else {
        return Expression(complexResult.real());
    }
};

Expression subneg(const std::vector<Expression> & args){
    
    bool complexArg = false;
    
    double realResult = 0;
    double imagResult = 0;
    
    // preconditions
    if(nargs_equal(args,1)){
        if(args[0].isHeadNumber()){
            realResult = -args[0].head().asNumber();
        } else if (args[0].isHeadComplex()){
            complexArg = true;
            
            realResult = -args[0].head().asComplex().real();
            imagResult = -args[0].head().asComplex().imag();
        }
        else{
            throw SemanticError("Error in call to negate: invalid argument.");
        }
    }
    else if(nargs_equal(args,2)){
        if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
            realResult = args[0].head().asNumber() - args[1].head().asNumber();
        } else if( args[0].isHeadComplex() && !args[1].isHeadComplex()){
            complexArg = true;
            
            realResult = args[0].head().asComplex().real() - args[1].head().asNumber();
            imagResult = args[0].head().asComplex().imag();
        } else if ( !args[0].isHeadComplex() && args[1].isHeadComplex() ){
            complexArg = true;
            
            realResult = args[0].head().asNumber() - args[1].head().asComplex().real();
            imagResult = -args[1].head().asComplex().imag();
            
        } else {
            complexArg = true;
            
            realResult = args[0].head().asComplex().real() - args[1].head().asComplex().real();
            imagResult = args[0].head().asComplex().imag() - args[1].head().asComplex().imag();
        }
    }
    else{
        throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
    }
    if(complexArg){
        return Expression(std::complex<double>(realResult, imagResult));
    } else {
        return Expression(realResult);
    }
};

Expression div(const std::vector<Expression> & args){
    
    bool complexArg = false;
    
    std::complex<double> complexResult;
    
    if(nargs_equal(args,2)){
        if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
            complexResult.real(args[0].head().asNumber() / args[1].head().asNumber());
        } else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())){
            complexArg = true;
            
            complexResult = args[0].head().asComplex() / args[1].head().asNumber();
        }  else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())){
            complexArg = true;
            
            complexResult = args[0].head().asNumber() / args[1].head().asComplex();
        } else {
            throw SemanticError("Error in call to division: invalid argument.");
        }
    }
    else{
        throw SemanticError("Error in call to division: invalid number of arguments.");
    }
    
    if(complexArg){
        return Expression(complexResult);
    } else {
        return Expression(complexResult.real());
    }
};

Expression sqrt(const std::vector<Expression> & args){
    
    bool complexArg = false;
    
    double realResult = 0;
    std::complex<double> complexResult;
    
    if(nargs_equal(args,1)){
        if (args[0].isHeadNumber() && args[0].head().asNumber() >= 0){
            realResult = std::sqrt(args[0].head().asNumber());
        } else if (args[0].isHeadNumber()){
            complexArg = true;
            
            complexResult = std::sqrt(std::complex<double>(args[0].head().asNumber(), 0));
        } else {
            complexArg = true;
            
            complexResult = std::sqrt(args[0].head().asComplex());
            
        }
    }
    else {
        throw SemanticError("Error in call to sqaure root: invalid number of arguments");
    }
    
    if(complexArg){
        
        if (complexResult.real() < 1e-15) {
            complexResult.real(0);
        }
        
        if (complexResult.imag() < 1e-15) {
            complexResult.imag(0);
        }
        
        return Expression(complexResult);
    } else {
        return Expression(realResult);
    }
};

Expression power(const std::vector<Expression> & args){
    
    bool complexArg = false;
    
    std::complex<double> complexResult;
    
    if(nargs_equal(args,2)){
        if (args[0].isHeadComplex() && args[1].isHeadNumber()){
            complexArg = true;
            
            complexResult = pow(args[0].head().asComplex(), args[1].head().asNumber());
        } else if (args[0].isHeadNumber() && args[1].isHeadComplex()){
            complexArg = true;
            
            complexResult = pow(args[0].head().asNumber(), args[1].head().asComplex());
        } else {
            complexResult = pow(args[0].head().asNumber(), args[1].head().asNumber());
        }
    } else {
        throw SemanticError("Error in call to power: invalid number of arguments");
    }
    
    if(complexArg){
        if (complexResult.real() < 1e-15 && complexResult.real() > -1e-15) {
            complexResult.real(0);
        }
        
        if (complexResult.imag() < 1e-15 && complexResult.imag() > -1e-15) {
            complexResult.imag(0);
        }
        
        return Expression(complexResult);
    } else {
        return Expression(complexResult.real());
    }
};

Expression ln(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        if (args[0].head().asNumber() >= 0){
            // Natural log
            result = log(args[0].head().asNumber());
        }
        else {
            throw SemanticError("Error in call to natural log: invalid (negative) argument.");
        }
    }
    else {
        throw SemanticError("Error in call to natural log: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression sin(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        result = sin(args[0].head().asNumber());
    }
    else {
        throw SemanticError("Error in call to sin: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression cos(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        result = cos(args[0].head().asNumber());
    }
    else {
        throw SemanticError("Error in call to cos: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression tan(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        result = tan(args[0].head().asNumber());
    }
    else {
        throw SemanticError("Error in call to tan: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression realPart(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadComplex()){
            result = args[0].head().asComplex().real();
        } else {
            throw SemanticError("Error in call to real: number is not complex");
        }
    }
    else {
        throw SemanticError("Error in call to real: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression imagPart(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadComplex()){
            result = args[0].head().asComplex().imag();
        } else {
            throw SemanticError("Error in call to imag: number is not complex");
        }
    }
    else {
        throw SemanticError("Error in call to imag: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression mag(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadComplex()){
            result = std::abs(args[0].head().asComplex());
        } else {
            throw SemanticError("Error in call to mag: number is not complex");
        }
    }
    else {
        throw SemanticError("Error in call to mag: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression arg(const std::vector<Expression> & args){
    
    double result = 0;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadComplex()){
            result = std::arg(args[0].head().asComplex());
        } else {
            throw SemanticError("Error in call to arg: number is not complex");
        }
    }
    else {
        throw SemanticError("Error in call to arg: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression conj(const std::vector<Expression> & args){
    
    std::complex<double> result = 0;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadComplex()){
            result = std::conj(args[0].head().asComplex());
        } else {
            throw SemanticError("Error in call to conj: number is not complex");
        }
    }
    else {
        throw SemanticError("Error in call to conj: invalid number of arguments");
    }
    
    return Expression(result);
};

Expression first(const std::vector<Expression> & args){
    
    Expression result;
    
    std::list<Expression> results;
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadList()){
            for (Expression::ConstIteratorType i = args[0].tailConstBegin(); i != args[0].tailConstEnd(); i++){
                results.push_back(*i);
            }
            if (!results.empty()){
                result = results.front();
            } else {
                throw SemanticError("Error: argument to first is an empty list");
            }
        } else {
            throw SemanticError("Error: argument to first is not a list");
        }
    }
    else {
        throw SemanticError("Error: more than one argument in call to first");
    }
    
    return result;
};

Expression rest(const std::vector<Expression> & args){
    
    bool listEmpty = true;
    Expression result;
    result.setHead(Atom("list"));
    
    if(nargs_equal(args,1)){
        if(args[0].isHeadList()){
            for (Expression::ConstIteratorType i = args[0].tailConstBegin(); i != args[0].tailConstEnd(); i++){
                if (i != args[0].tailConstBegin()){
                    result.append(*i);
                }
                listEmpty = false;
            }
            
            if(listEmpty){
                throw SemanticError("Error: argument to rest is an empty list");
            }
        } else {
            throw SemanticError("Error: argument to rest is not a list");
        }
    }
    else {
        throw SemanticError("Error: more than one argument in call to rest");
    }
    
    return result;
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I (0.0, 1.0);

Environment::Environment(){
    
    reset();
}

bool Environment::is_known(const Atom & sym) const{
    if(!sym.isSymbol()) return false;
    
    return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
    if(!sym.isSymbol()) return false;
    
    auto result = envmap.find(sym.asSymbol());
    return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{
    
    Expression exp;
    
    if(sym.isSymbol()){
        auto result = envmap.find(sym.asSymbol());
        if((result != envmap.end()) && (result->second.type == ExpressionType)){
            exp = result->second.exp;
        }
    }
    
    return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){
    
    if(!sym.isSymbol()){
        throw SemanticError("Attempt to add non-symbol to environment");
    }
    
    // error if overwriting symbol map
    if(envmap.find(sym.asSymbol()) != envmap.end()){
        throw SemanticError("Attempt to overwrite symbol in environemnt");
    }
    
    envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const{
    if(!sym.isSymbol()) return false;
    
    auto result = envmap.find(sym.asSymbol());
    return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{
    
    //Procedure proc = default_proc;
    
    if(sym.isSymbol()){
        auto result = envmap.find(sym.asSymbol());
        if((result != envmap.end()) && (result->second.type == ProcedureType)){
            return result->second.proc;
        }
    }
    
    return default_proc;
}

/*
 Reset the environment to the default state. First remove all entries and
 then re-add the default ones.
 */
void Environment::reset(){
    
    envmap.clear();
    
    // Built-In value of pi
    envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));
    
    // Built-In value of e
    envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));
    
    // Built-In value of I
    envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));
    
    // Procedure: add;
    envmap.emplace("+", EnvResult(ProcedureType, add));
    
    // Procedure: subneg;
    envmap.emplace("-", EnvResult(ProcedureType, subneg));
    
    // Procedure: mul;
    envmap.emplace("*", EnvResult(ProcedureType, mul));
    
    // Procedure: div;
    envmap.emplace("/", EnvResult(ProcedureType, div));
    
    // Procedure: sqrt;
    envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));
    
    // Procedure: pow;
    envmap.emplace("^", EnvResult(ProcedureType, power));
    
    // Procedure: ln;
    envmap.emplace("ln", EnvResult(ProcedureType, ln));
    
    // Procedure: sin;
    envmap.emplace("sin", EnvResult(ProcedureType, sin));
    
    // Procedure: cos;
    envmap.emplace("cos", EnvResult(ProcedureType, cos));
    
    // Procedure: tan;
    envmap.emplace("tan", EnvResult(ProcedureType, tan));
    
    // Procedure: real;
    envmap.emplace("real", EnvResult(ProcedureType, realPart));
    
    // Procedure: imag;
    envmap.emplace("imag", EnvResult(ProcedureType, imagPart));
    
    // Procedure: mag;
    envmap.emplace("mag", EnvResult(ProcedureType, mag));
    
    // Procedure: arg;
    envmap.emplace("arg", EnvResult(ProcedureType, arg));
    
    // Procedure: conj;
    envmap.emplace("conj", EnvResult(ProcedureType, conj));
    
    // Procedure: first;
    envmap.emplace("first", EnvResult(ProcedureType, first));
    
    // Procedure: rest;
    envmap.emplace("rest", EnvResult(ProcedureType, rest));
}

#include "expression.hpp"

#include <sstream>
#include <list>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(){}

Expression::Expression(const Atom & a){
    
    m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a){
    
    m_head = a.m_head;
    for(auto e : a.m_tail){
        m_tail.push_back(e);
    }
    
    properties = a.properties;
}

Expression & Expression::operator=(const Expression & a){
    
    // prevent self-assignment
    if(this != &a){
        m_head = a.m_head;
        m_tail.clear();
        for(auto e : a.m_tail){
            m_tail.push_back(e);
        }
        
        properties = a.properties;
    }
    
    return *this;
}


Atom & Expression::head(){
    return m_head;
}

const Atom & Expression::head() const{
    return m_head;
}

bool Expression::isHeadNumber() const noexcept{
    return m_head.isNumber();
}

bool Expression::isHeadComplex() const noexcept{
    return m_head.isComplex();
}

bool Expression::isHeadSymbol() const noexcept{
    return m_head.isSymbol();
}

bool Expression::isHeadList() const noexcept{
    return m_head.isList();
}

bool Expression::isHeadString() const noexcept{
    return m_head.isUserString();
}

bool Expression::isHeadLambda() const noexcept{
    return m_head.isLambda();
}

bool Expression::isHeadNone() const noexcept{
    return m_head.isNone();
}

bool Expression::isHeadPoint() const noexcept{
    auto result = properties.find("\"object-name\"");
    
    if (result == properties.end()){
        return false;
    } else if (result->second.head().asSymbol() == "\"point\""){
        return true;
    } else {
        return false;
    }
}

bool Expression::isHeadLine() const noexcept{
    auto result = properties.find("\"object-name\"");
    
    if (result == properties.end()){
        return false;
    } else if (result->second.head().asSymbol() == "\"line\""){
        return true;
    } else {
        return false;
    }
}

bool Expression::isHeadText() const noexcept{
    auto result = properties.find("\"object-name\"");
    
    if (result == properties.end()){
        return false;
    } else if (result->second.head().asSymbol() == "\"text\""){
        return true;
    } else {
        return false;
    }
}

void Expression::setHead(const Atom & a){
    m_head = a;
}

void Expression::append(const Atom & a){
    m_tail.emplace_back(a);
}

void Expression::append(const Expression & a){
    m_tail.emplace_back(a);
}

void Expression::add_property(const Expression & key, const Expression & value) {
    // Check if key already exists
    auto result = properties.find(key.head().asSymbol());
    
    if (result != properties.end()){
        properties.erase(key.head().asSymbol());
        properties.emplace(key.head().asSymbol(), value);
    } else {
        properties.emplace(key.head().asSymbol(), value);
    }
    
    
}

Expression Expression::get_property(const Expression & key){
    
    auto result = properties.find(key.head().asSymbol());
    
    if (result != properties.end()){
        return result->second;
    } else {
        return Expression();
    }
    
}

Expression * Expression::tail(){
    Expression * ptr = nullptr;
    
    if(m_tail.size() > 0){
        ptr = &m_tail.back();
    }
    
    return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
    return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
    return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, Environment & env){
    
    // head must be a symbol
    if(!op.isSymbol()){
        throw SemanticError("Error during evaluation: procedure name not symbol");
    }
    
    // must map to a proc
    if(!env.is_proc(op)){
        throw SemanticError("Error during evaluation: symbol does not name a procedure");
    }
    
    if (env.is_lambda(op)){
        Expression passToApply;
        passToApply.setHead(Atom("apply"));
        passToApply.append(op);
        
        Expression expArguments;
        expArguments.setHead(Atom("list"));
        
        for (auto arg: args){
            expArguments.append(arg);
        }
        passToApply.append(expArguments);
        
        return passToApply.eval(env);
    } else {
        // map from symbol to proc
        Procedure proc = env.get_proc(op);
        
        // call proc with args
        return proc(args);
    }
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
        if(env.is_exp(head)){
            return env.get_exp(head);
        }
        else{
            throw SemanticError("Error during evaluation: unknown symbol");
        }
    }
    else if(head.isNumber()){
        return Expression(head);
    }
    else{
        throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
    
    if(m_tail.size() == 0){
        throw SemanticError("Error during evaluation: zero arguments to begin");
    }
    
    // evaluate each arg from tail, return the last
    Expression result;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
        result = it->eval(env);
    }
    
    return result;
}


Expression Expression::handle_define(Environment & env){
    
    // tail must have size 3 or error
    if(m_tail.size() != 2){
        throw SemanticError("Error during evaluation: invalid number of arguments to define");
    }
    
    // tail[0] must be symbol
    if(!m_tail[0].isHeadSymbol()){
        throw SemanticError("Error during evaluation: first argument to define not symbol");
    }
    
    // but tail[0] must not be a special-form or procedure
    std::string s = m_tail[0].head().asSymbol();
    if((s == "define") || (s == "begin")){
        throw SemanticError("Error during evaluation: attempt to redefine a special-form");
    }
    
    if(env.is_proc(m_head)){
        throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
    }
    
    // eval tail[1]
    Expression result = m_tail[1].eval(env);
    
    if (!m_tail[1].isHeadLambda()){
        if(env.is_exp(m_head)){
            throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
        }
        
        //and add to env
        env.add_exp(m_tail[0].head(), result);
    } else {
        // Procedure proc;
        /**
        for(Expression::IteratorType it = m_tail[1].m_tail.begin(); it != m_tail[1].m_tail.end(); ++it){
            proc = env.get_proc(it->head());
        }
         */
        // proc = env.get_proc(m_tail[1].m_tail[1].head());
        // env.add_proc(m_tail[0].head(), proc);
        env.add_proc(m_tail[0].head(), result);
    }
    
    return result;
}

Expression Expression::handle_list(Environment & env){
    
    Expression result = m_head;
    
    if(m_tail.size() == 0){
        return result;
    } else {
        for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
            result.append(it->eval(env));
        }
        return result;
    }
    
}

Expression Expression::handle_lambda(){
    Expression result;
    Expression arguments;
    Expression expression;
    
    result.setHead(Atom("lambda"));
    arguments.setHead(Atom("list"));
    
    // must have two arguments
    if(m_tail.size() != 2){
        throw SemanticError("Error during evaluation: invalid number of arguments to lambda");
    } else {
        arguments.append(m_tail[0].head());
        for(Expression::IteratorType it = m_tail[0].m_tail.begin(); it != m_tail[0].m_tail.end(); ++it){
            arguments.append(*it);
        }
        
        expression = m_tail[1];
        
    }
    
    result.append(arguments);
    result.append(expression);
    
    return result;
}

Expression Expression::handle_apply(Environment & env){
    Expression result;
    Expression expression;
    
    if (!env.is_proc(m_tail[0].head()) || m_tail[0].m_tail.size() != 0){
        throw SemanticError("Error: first argument to apply not a procedure");
    }
    
    if (!m_tail[1].isHeadList()){
        throw SemanticError("Error: second argument to apply not a list");
    }
    
    // must have two arguments
    if(m_tail.size() != 2){
        throw SemanticError("Error during evaluation: invalid number of arguments to apply");
    } else {
        if (env.is_lambda(m_tail[0].head())){
            Expression exp = env.get_exp(m_tail[0].head());
            
            std::vector<Atom> identifiers;
            std::vector<Expression> values;
            std::vector<Expression> results;
            
            for(Expression::IteratorType it = exp.m_tail[0].m_tail.begin(); it != exp.m_tail[0].m_tail.end(); ++it){
                if (it->isHeadSymbol()){
                    identifiers.push_back(it->m_head.asSymbol());
                }
            }
            
            for(Expression::IteratorType it = m_tail[1].m_tail.begin(); it != m_tail[1].m_tail.end(); ++it){
                values.push_back(it->eval(env));
            }
            
            if (identifiers.size() == values.size()){
                for (size_t i = 0; i < identifiers.size(); i++){
                    env.add_exp(identifiers.at(i), values.at(i));
                }
            } else {
                throw SemanticError("Error: during apply: Error in call to procedure: invalid number of arguments.");
            }
            
            result = exp.m_tail[1].eval(env);
        } else {
            // Procedures other than lambda
            std::vector<Expression> results;
            for(Expression::IteratorType it = m_tail[1].m_tail.begin(); it != m_tail[1].m_tail.end(); ++it){
                // expression.append(*it);
                results.push_back(it->eval(env));
            }
            
            result = apply(m_tail[0].head(), results, env);
        }
    
    }
    
    return result;
}

Expression Expression::handle_map(Environment & env){
    Expression results;
    Expression currentResult;
    
    results.setHead(Atom("list"));
    
    if (!env.is_proc(m_tail[0].head()) || m_tail[0].m_tail.size() != 0){
        throw SemanticError("Error: first argument to map not a procedure");
    }
    
    if (!m_tail[1].isHeadList()){
        throw SemanticError("Error: second argument to map not a list");
    }
    
    // must have two arguments
    if(m_tail.size() != 2){
        throw SemanticError("Error during evaluation: invalid number of arguments to map");
    } else {
        
        for(Expression::IteratorType it = m_tail[1].m_tail.begin(); it != m_tail[1].m_tail.end(); ++it){
            
            Expression passToApply;
            passToApply.setHead(Atom("apply"));
            passToApply.append(m_tail[0]);
            passToApply.append((Atom("list")));
            passToApply.m_tail[1].append(*it);
            
            results.append(passToApply.eval(env));
        }
        
    }
    
    return results;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){
    
    // TODO: Deal with empty lambda
    if(m_tail.empty() && !m_head.isList() && !m_head.isLambda() && !m_head.isUserString()){
        return handle_lookup(m_head, env);
    }
    // handle begin special-form
    else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
        return handle_begin(env);
    }
    // handle define special-form
    else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
        return handle_define(env);
    }
    // handle apply special-form
    else if(m_head.isSymbol() && m_head.asSymbol() == "apply"){
        return handle_apply(env);
    }
    // handle map special-form
    else if(m_head.isSymbol() && m_head.asSymbol() == "map"){
        return handle_map(env);
    }
    // handle lambda special-form
    else if(m_head.isLambda()){
        return handle_lambda();
    }
    // handle list special-form
    else if(m_head.isList()){
        return handle_list(env);
    }
    else if(m_head.isUserString()){
        return *this;
    }
    // else attempt to treat as procedure
    else{ 
        std::vector<Expression> results;
        for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
            results.push_back(it->eval(env));
        }
        return apply(m_head, results, env);
    }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){
    
    /// Final output handled
    
    if (exp.head().isNone()){
        out << "NONE";
        return out;
    }
    
    if (!exp.isHeadComplex()){
        out << "(";  // Paranthesis added
    }
    
    std::string procs = "+, -, *, /, sin, cos, tan";
    
    if (!exp.isHeadList() && !exp.isHeadLambda()){
        out << exp.head();
        if (exp.isHeadSymbol() && procs.find(exp.head().asSymbol()) != std::string::npos){
            out << " ";
        }
    }
    
    for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
        out << *e;
        auto f = e;
        f++;
        if (f != exp.tailConstEnd()){
            out << " ";
        }
    }
    
    if (!exp.isHeadComplex()){
        out << ")";
    }
    
    return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{
    
    bool result;
    
    if(exp.isHeadComplex()) {
        result = (m_head.asComplex() == exp.m_head.asComplex());
    } else if(exp.isHeadList()){
        result = (m_head.asSymbol() == exp.m_head.asSymbol());
    } else {
        result = (m_head == exp.m_head);
    }
    
    result = result && (m_tail.size() == exp.m_tail.size());
    
    if(result){
        for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
            (lefte != m_tail.end()) && (righte != exp.m_tail.end());
            ++lefte, ++righte){
            result = result && (*lefte == *righte);
        }
    }
    
    return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{
    
    return !(left == right);
}

#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){
    
    setNumber(value);
    
}

Atom::Atom(std::complex<double> value){
    
    setComplex(value);
    
}

Atom::Atom(const Token & token): Atom(){
    
    // is token a number?
    double temp;
    std::istringstream iss(token.asString());
    if(iss >> temp){
        // check for trailing characters if >> succeeds
        if(iss.rdbuf()->in_avail() == 0){
            setNumber(temp);
        }
    }
    else{ // else assume symbol
        // make sure does not start with number
        if(token.type() == Token::USERSTRING){
            setUserString(token.asString());
        } else if(!std::isdigit(token.asString()[0])){
            setSymbol(token.asString());
        }
    }
}

Atom::Atom(const std::string & value): Atom() {
    
    setSymbol(value);
}

Atom::Atom(const Atom & x): Atom(){
    if(x.isNumber()){
        setNumber(x.complexValue.real());
    }
    else if(x.isSymbol()){
        setSymbol(x.stringValue);
    }
}

Atom & Atom::operator=(const Atom & x){
    
    if(this != &x){
        if(x.m_type == NoneKind){
            m_type = NoneKind;
        }
        else if(x.m_type == NumberKind){
            setNumber(x.complexValue.real());
        }
        else if(x.m_type == ComplexKind){
            setComplex(x.complexValue);
        }
        else if(x.m_type == SymbolKind || x.m_type == ListKind || x.m_type == LambdaKind){
            setSymbol(x.stringValue);
        } else if(x.m_type == UserStringKind){
            setUserString(x.stringValue);
        }
    }
    return *this;
}

Atom::~Atom(){
    
    // we need to ensure the destructor of the symbol string is called
    if(m_type == SymbolKind){
        stringValue.~basic_string();
    }
}

bool Atom::isNone() const noexcept{
    return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
    return m_type == NumberKind;
}

bool Atom::isComplex() const noexcept{
    return m_type == ComplexKind;
}

bool Atom::isSymbol() const noexcept{
    return m_type == SymbolKind;
}

bool Atom::isUserString() const noexcept{
    return m_type == UserStringKind;
}

bool Atom::isList() const noexcept{
    return m_type == ListKind;
}

bool Atom::isLambda() const noexcept{
    return m_type == LambdaKind;
}

void Atom::setNumber(double value){
    
    m_type = NumberKind;
    
    complexValue = std::complex<double>(value, 0.0);
}

void Atom::setComplex(std::complex<double> value){
    
    m_type = ComplexKind;
    
    complexValue = value;
}

void Atom::setSymbol(const std::string & value){
    
    // we need to ensure the destructor of the symbol string is called
    if(m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind){
        stringValue.~basic_string();
    }
    
    if(value == "list"){
        m_type = ListKind;
    } else if (value == "lambda"){
        m_type = LambdaKind;
    } else {
        m_type = SymbolKind;
    }
    
    // copy construct in place
    new (&stringValue) std::string(value);
}

void Atom::setUserString(const std::string & value){
    
    // we need to ensure the destructor of the symbol string is called
    if(m_type == UserStringKind){
        stringValue.~basic_string();
    }
    
    m_type = UserStringKind;
    
    // copy construct in place
    new (&stringValue) std::string(value);
}

double Atom::asNumber() const noexcept{
    
    return (m_type == NumberKind) ? complexValue.real() : 0.0;
}

std::complex<double> Atom::asComplex() const noexcept{
    
    return (m_type == ComplexKind) ? complexValue : std::complex<double> (0.0, 0.0);
}

std::string Atom::asSymbol() const noexcept{
    
    std::string result;
    
    if(m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind || m_type == UserStringKind){
        result = stringValue;
    }
    
    return result;
}

bool Atom::operator==(const Atom & right) const noexcept{
    
    if(m_type != right.m_type) return false;
    
    switch(m_type){
        case NoneKind:
            if(right.m_type != NoneKind) return false;
            break;
        case NumberKind:
        {
            if(right.m_type != NumberKind) return false;
            double dleft = complexValue.real();
            double dright = right.complexValue.real();
            double diff = fabs(dleft - dright);
            if(std::isnan(diff) ||
               (diff > std::numeric_limits<double>::epsilon())) return false;
        }
            break;
        case SymbolKind:
        {
            if(right.m_type != SymbolKind) return false;
            
            return stringValue == right.stringValue;
        }
            break;
        case UserStringKind:
        {
            if(right.m_type != UserStringKind) return false;
            
            return stringValue == right.stringValue;
        }
            break;
        default:
            return false;
    }
    
    return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
    
    return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){
    
    if(a.isNumber()){
        out << a.asNumber();
    }
    if(a.isComplex()){
        out << a.asComplex();
    }
    if(a.isSymbol() || a.isLambda() || a.isUserString()){
        out << a.asSymbol();
    }
    return out;
}

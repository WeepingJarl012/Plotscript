#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
    
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    if(!ok){
        std::cerr << "Failed to parse: " << program << std::endl;
    }
    REQUIRE(ok == true);
    
    Expression result;
    REQUIRE_NOTHROW(result = interp.evaluate());
    
    return result;
}

Expression runError(const std::string & program){
    
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    if(!ok){
        std::cerr << "Failed to parse: " << program << std::endl;
    }
    REQUIRE(ok == true);
    
    Expression result;
    REQUIRE_THROWS(result = interp.evaluate());
    
    return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {
    
    std::string program = "(begin (define r 10) (* pi (* r r)))";
    
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {
    
    std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
    
    for(auto program : programs){
        std::istringstream iss(program);
        
        Interpreter interp;
        
        bool ok = interp.parseStream(iss);
        
        REQUIRE(ok == true);
    }
    
    {
        std::istringstream iss("(define x 1abc)");
        
        Interpreter interp;
        
        bool ok = interp.parseStream(iss);
        
        REQUIRE(ok == false);
    }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {
    
    {
        std::string program = "(f";
        std::istringstream iss(program);
        
        Interpreter interp;
        bool ok = interp.parseStream(iss);
        REQUIRE(ok == false);
    }
    
    {
        std::string program = "(begin (define r 10) (* pi (* r r";
        std::istringstream iss(program);
        
        Interpreter interp;
        bool ok = interp.parseStream(iss);
        REQUIRE(ok == false);
    }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {
    
    std::string program = "(begin (define r 10) (* pi (* r r))) )";
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {
    
    std::string program = "hello";
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {
    
    std::string program;
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {
    
    std::string program = "( )";
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {
    
    std::string program = "(1abc)";
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {
    
    std::string program = "(+ 1 2) (+ 3 4)";
    std::istringstream iss(program);
    
    Interpreter interp;
    
    bool ok = interp.parseStream(iss);
    
    REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
    
    { // Number
        std::string program = "(4)";
        Expression result = run(program);
        REQUIRE(result == Expression(4.));
    }
    
    { // Symbol
        std::string program = "(pi)";
        Expression result = run(program);
        REQUIRE(result == Expression(atan2(0, -1)));
    }
    
}

// TO-DO: Update tests

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {
    
    { // add, binary case
        std::string program = "(+ 1 2)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(3.));
    }
    
    { // add, throw error
        std::string program = "(+ a 2)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // add, complex binary case
        std::string program = "(+ I 2)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(2, 1)));
    }
    
    { // add, 3-ary case
        std::string program = "(+ 1 2 3)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(6.));
    }
    
    { // add, complex 3-ary case
        std::string program = "(+ I 2 3)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(5, 1)));
    }
    
    { // add, 6-ary case
        std::string program = "(+ 1 2 3 4 5 6)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(21.));
    }
    
    { // add, complex 6-ary case
        std::string program = "(+ I 2 3 4 5 6)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(20, 1)));
    }
}

TEST_CASE("Test Interpreter result with simple procedures (mul)", "[interpreter]"){
    
    { // mul, simple binary case
        std::string program = "(* 2 1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(2.0));
    }
    
    { // mul, throw error
        std::string program = "(* a 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // mul, complex binary case
        std::string program = "(* I 1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0,1)));
    }
    
    { // mul, complex binary case
        std::string program = "(- I (sqrt -1))";
        INFO(program);
        Expression result = run(program);
        std::complex<double> complexNumber (0.0, 0.0);
        REQUIRE(result == Expression(complexNumber));
    }
}

TEST_CASE("Test Interpreter result with simple procedures (subneg)", "[interpreter]"){
    
    { // subneg, negate complex unary case
        std::string program = "(- I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0,-1)));
    }
    
    { // subneg, throw error
        std::string program = "(- a)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // subneg, subtract complex binary case
        std::string program = "(- I 1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(-1,1)));
    }
    
    { // subneg, subtract complex binary case
        std::string program = "(- 1 I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(1,-1)));
    }
    
    { // subneg, subtract complex binary case
        std::string program = "(- I I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0,0)));
    }
}

// Test new expressions here

TEST_CASE( "Test Interpreter result with simple procedures (sqrt)", "[interpreter]" ) {
    
    { // sqrt, simple unary case
        std::string program = "(sqrt 4)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(2.));
    }
    
    { // sqrt, throw error
        std::string program = "(sqrt 4 4)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // sqrt, simple unnary case complex result
        std::string program = "(sqrt -1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0, 1)));
    }
    
    { // sqrt, simple unnary case complex result complex input
        std::string program = "(sqrt (* 0 I))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0, 0)));
    }
    
    // Add testing for throwing of semantic error
}

TEST_CASE( "Test Interpreter result with simple procedures (div)", "[interpreter]" ) {
    
    { // div, simple binary case
        std::string program = "(/ 4 2)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(2.));
    }
    
    { // div, throw error
        std::string program = "(/ a 2)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // div, throw error
        std::string program = "(/ 2 2 2)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // div, first arg complex second arg double
        std::string program = "(/ I 1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0, 1)));
    }
    
    { // div, complex binary case
        std::string program = "(/ 3 I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(0, -3)));
    }
    
    // Add testing for throwing of semantic error
}

TEST_CASE( "Test Interpreter result with simple procedures (pow)", "[interpreter]" ) {
    
    { // pow, simple binary case
        std::string program = "(^ 3 2)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(9.));
    }
    
    { // pow, throw error
        std::string program = "(^ 3)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // pow, complex binary case
        std::string program = "(^ I 2)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(-1, 0)));
    }
    
    { // pow, complex binary case
        std::string program = "(^ 2 (* I 0))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double>(1, 0)));
    }
    
    // Add testing for throwing of semantic error
}

TEST_CASE( "Test Interpreter result with simple procedures (ln)", "[interpreter]" ) {
    
    { // log, simple unary case
        std::string program = "(ln 1)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(0.));
    }
    
    { // log, throw error
        std::string program = "(ln -1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // log, throw error
        std::string program = "(ln 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
}

TEST_CASE( "Test Interpreter result with simple procedures (sin)", "[interpreter]" ) {
    
    { // sin, simple unary case
        std::string program = "(sin pi)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(0.));
    }
    
    { // sin, throw error
        std::string program = "(sin 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (cos)", "[interpreter]" ) {
    
    { // cos, simple unary case
        std::string program = "(cos pi)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(-1.));
    }
    
    { // cos, throw error
        std::string program = "(cos 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (tan)", "[interpreter]" ) {
    
    { // tan, simple unary case
        std::string program = "(tan pi)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(0.));
    }
    
    { // tan, throw error
        std::string program = "(tan 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
}

TEST_CASE( "Test Interpreter result with simple procedures (real)", "[interpreter]" ) {
    
    { // real, simple unary case
        std::string program = "(real I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(0));
    }
    
    { // real, throw error
        std::string program = "(real 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // real, throw error
        std::string program = "(real 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (imag)", "[interpreter]" ) {
    
    { // imag, simple unary case
        std::string program = "(imag I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(1));
    }
    
    { // imag, throw error
        std::string program = "(imag 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // imag, throw error
        std::string program = "(imag 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (mag)", "[interpreter]" ) {
    
    { // mag, simple unary case
        std::string program = "(mag I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(1));
    }
    
    { // mag, throw error
        std::string program = "(mag 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // mag, throw error
        std::string program = "(mag 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (arg)", "[interpreter]" ) {
    
    { // arg, simple unary case
        std::string program = "(arg I)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult = Expression(1.5707963267948966);
        REQUIRE(result == expectedResult);
    }
    
    { // arg, simple unary case
        std::string program = "(arg (- I (sqrt -1)))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(0));
    }
    
    { // arg, throw error
        std::string program = "(arg 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // arg, throw error
        std::string program = "(arg 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (conj)", "[interpreter]" ) {
    
    { // conj, simple unary case
        std::string program = "(conj I)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(std::complex<double> (0,-1)));
    }
    
    { // conj, throw error
        std::string program = "(conj 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
    { // conj, throw error
        std::string program = "(conj 1 1)";
        INFO(program);
        Expression result = runError(program);
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (first)", "[interpreter]" ) {
    
    { // first, simple case of first
        std::string program = "(first (list 1 2 3))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(1));
    }
    
    { // first, throws semantic error when list is empty
        Interpreter interp;
        std::string program = "(first (list))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // first, throws semantic error when multiple args
        Interpreter interp;
        std::string program = "(first (list 1) (list 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // first, throws semantic error when arg is not a list
        Interpreter interp;
        std::string program = "(first 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (rest)", "[interpreter]" ) {
    
    { // rest, simple case of rest
        std::string program = "(rest (list 1 2 3))";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(2));
        expectedResult.append(Atom(3));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // rest, throws semantic error when list is empty
        Interpreter interp;
        std::string program = "(rest (list))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // rest, throws semantic error when multiple args
        Interpreter interp;
        std::string program = "(rest (list 1) (list 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // rest, throws semantic error when arg is not a list
        Interpreter interp;
        std::string program = "(rest 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (length)", "[interpreter]" ) {
    
    { // length, simple case of length
        std::string program = "(length (list 1 2 3))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(3));
    }
    
    { // length, throws semantic error when argument is not a list
        Interpreter interp;
        std::string program = "(length (1 + 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // length, throws semantic error when multiple args
        Interpreter interp;
        std::string program = "(length (list 1) (list 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // length, throws semantic error when arg is not a list
        Interpreter interp;
        std::string program = "(length 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (append)", "[interpreter]" ) {
    
    { // append, simple case of append with complex
        std::string program = "(append (list 1 2 3) (+ 3 (* 4 I)))";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(1));
        expectedResult.append(Atom(2));
        expectedResult.append(Atom(3));
        std::complex<double> complexNum (3, 4);
        expectedResult.append(Atom(complexNum));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // append, simple case of append with double
        std::string program = "(append (list 1 2 3) 3)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(1));
        expectedResult.append(Atom(2));
        expectedResult.append(Atom(3));
        expectedResult.append(Atom(3));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // append, throws semantic error when argument is not a list
        Interpreter interp;
        std::string program = "(append 1 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // append, throws semantic error when wrong number of args given
        Interpreter interp;
        std::string program = "(append 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (join)", "[interpreter]" ) {
    
    { // join, simple case of join
        std::string program = "(join (list 1 2 3) (list 4 5))";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(1));
        expectedResult.append(Atom(2));
        expectedResult.append(Atom(3));
        expectedResult.append(Atom(4));
        expectedResult.append(Atom(5));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // join, throws semantic error when either argument is not a list
        Interpreter interp;
        std::string program = "(join 1 (list 1 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // join, throws semantic error when wrong number of args given
        Interpreter interp;
        std::string program = "(join (list 1 2))";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (range)", "[interpreter]" ) {
    
    { // range, simple case of range
        std::string program = "(range 0 5 1)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(0));
        expectedResult.append(Atom(1));
        expectedResult.append(Atom(2));
        expectedResult.append(Atom(3));
        expectedResult.append(Atom(4));
        expectedResult.append(Atom(5));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // range, throws semantic error when first argument is less than second
        Interpreter interp;
        std::string program = "(range 3 -1 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // range, throws semantic error when any argument is not a number
        Interpreter interp;
        std::string program = "(range I -1 1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // range, throws semantic error when increment is not positive
        Interpreter interp;
        std::string program = "(range 0 5 -1)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
    { // range, throws semantic error when wrong number of args given
        Interpreter interp;
        std::string program = "(range 0 5)";
        INFO(program);
        Expression result = runError(program);
        REQUIRE(result == Expression());
    }
    
}

TEST_CASE( "Test Interpreter result with simple procedures (lambda)", "[interpreter]" ) {
    
    { // lambda, simple lambda equation
        std::string program = "(lambda (x y) (/ x y))";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("lambda"));
        
        Expression expectedArguments;
        expectedArguments.setHead(Atom("list"));
        expectedArguments.append(Atom("x"));
        expectedArguments.append(Atom("y"));
        
        Expression expectedExpression;
        expectedExpression.setHead(Atom("/"));
        expectedExpression.append(Atom("x"));
        expectedExpression.append(Atom("y"));
        
        expectedResult.append(expectedArguments);
        expectedResult.append(expectedExpression);
        
        REQUIRE(result == expectedResult);
    }
}

TEST_CASE( "Test Interpreter result with simple procedures (list)", "[interpreter]" ) {
    
    { // list, simple empty list
        std::string program = "(list)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        REQUIRE(result == expectedResult);
    }
    
    { // list, simple list with one real number
        std::string program = "(list 1)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(1));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // list, simple list with one complex number
        std::string program = "(list I)";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom(std::complex<double>(0,1)));
        REQUIRE(result == Expression(expectedResult));
    }
    
    { // list, list of list
        std::string program = "(list (list))";
        INFO(program);
        Expression result = run(program);
        Expression expectedResult;
        expectedResult.setHead(Atom("list"));
        expectedResult.append(Atom("list"));
        REQUIRE(result == Expression(expectedResult));
    }
}

TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {
    
    {
        std::string program = "(define answer 42)";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(42.));
    }
    
    { // begin, throw error
        std::string program = "(begin)";
        INFO(program);
        Expression result = runError(program);
    }
    
    {
        std::string program = "(begin (define answer 42)\n(answer))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(42.));
    }
    
    {
        std::string program = "(begin (define answer (+ 9 11)) (answer))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(20.));
    }
    
    {
        std::string program = "(begin (define a 1) (define b 1) (+ a b))";
        INFO(program);
        Expression result = run(program);
        REQUIRE(result == Expression(2.));
    }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {
    
    {
        std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
        Expression result = run(program);
        REQUIRE(result == Expression(43.));
    }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {
    
    {
        std::vector<std::string> programs = {"(+ 1 -2)",
            "(+ -3 1 1)",
            "(- 1)",
            "(- 1 2)",
            "(* 1 -1)",
            "(* 1 1 -1)",
            "(/ -1 1)",
            "(/ 1 -1)"};
        
        for(auto s : programs){
            Expression result = run(s);
            REQUIRE(result == Expression(-1.));
        }
    }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
    
    std::vector<std::string> programs = {"(@ none)", // so such procedure
        "(- 1 1 2)", // too many arguments
        "(define begin 1)", // redefine special form
        "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
        Interpreter interp;
        
        std::istringstream iss(s);
        
        bool ok = interp.parseStream(iss);
        REQUIRE(ok == true);
        
        REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {
    
    std::string input = R"(
    (+ 1 a)
    )";
    
    Interpreter interp;
    
    std::istringstream iss(input);
    
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);
    
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {
    
    std::string input = R"(
    (define a 1 2)
    )";
    
    Interpreter interp;
    
    std::istringstream iss(input);
    
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);
    
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
    (1 2 3)
    )";
    
    Interpreter interp;
    
    std::istringstream iss(input);
    
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);
    
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

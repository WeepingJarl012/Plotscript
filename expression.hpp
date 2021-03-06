/*! \file expression.hpp
 Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
 \brief An expression is a tree of Atoms.
 
 An expression is an atom called the head followed by a (possibly empty)
 list of expressions called the tail.
 */
class Expression {
public:
    
    typedef std::vector<Expression>::const_iterator ConstIteratorType;
    
    /// Default construct and Expression, whose type in NoneType
    Expression();
    
    /*! Construct an Expression with given Atom as head an empty tail
     \param atom the atom to make the head
     */
    Expression(const Atom & a);
    
    /// deep-copy construct an expression (recursive)
    Expression(const Expression & a);
    
    /// deep-copy assign an expression  (recursive)
    Expression & operator=(const Expression & a);
    
    /// return a reference to the head Atom
    Atom & head();
    
    /// return a const-reference to the head Atom
    const Atom & head() const;
    
    /// set head equal to atom
    void setHead(const Atom & a);
    
    /// append Atom to tail of the expression
    void append(const Atom & a);
    
    /// append Expression to tail of the expression
    void append(const Expression & a);
    
    /// return the current size of the tail
    int tailSize() const noexcept;
    
    /// Add a new property to an expression
    void add_property(const Expression & key, const Expression & value);
    
    /// Gets property value
    Expression get_property(const Expression & key);
    
    /// return a pointer to the last expression in the tail, or nullptr
    Expression * tail();
    
    /// return a const-iterator to the beginning of tail
    ConstIteratorType tailConstBegin() const noexcept;
    
    /// return a const-iterator to the tail end
    ConstIteratorType tailConstEnd() const noexcept;
    
    /// convienience member to determine if head atom is a number
    bool isHeadNumber() const noexcept;
    
    /// convenience member to determine if head atom is complex
    bool isHeadComplex() const noexcept;
    
    /// convienience member to determine if head atom is a symbol
    bool isHeadSymbol() const noexcept;
    
    /// convienience member to determine if head atom is a list
    bool isHeadList() const noexcept;
    
    /// convienience member to determine if head atom is a string
    bool isHeadString() const noexcept;
    
    /// convienience member to determine if head atom is a lambda
    bool isHeadLambda() const noexcept;
    
    /// convienience member to determine if head atom is a NONE
    bool isHeadNone() const noexcept;
    
    /// convienience member to determine if head atom is a point
    bool isHeadPoint() const noexcept;
    
    /// convienience member to determine if head atom is a line
    bool isHeadLine() const noexcept;
    
    /// convienience member to determine if head atom is a text
    bool isHeadText() const noexcept;
    
    /// Evaluate expression using a post-order traversal (recursive)
    Expression eval(Environment & env);
    
    /// equality comparison for two expressions (recursive)
    bool operator==(const Expression & exp) const noexcept;
    
private:
    
    // the head of the expression
    Atom m_head;
    
    // the tail list is expressed as a vector for access efficiency
    // and cache coherence, at the cost of wasted memory.
    std::vector<Expression> m_tail;
    
    // the property map
    std::map<std::string, Expression> properties;
    
    // convenience typedef
    typedef std::vector<Expression>::iterator IteratorType;
    
    // internal helper methods
    Expression handle_lookup(const Atom & head, const Environment & env);
    Expression handle_define(Environment & env);
    Expression handle_begin(Environment & env);
    Expression handle_list(Environment & env);
    Expression handle_lambda();
    Expression handle_apply(Environment & env);
    Expression handle_map(Environment & env);
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

#endif

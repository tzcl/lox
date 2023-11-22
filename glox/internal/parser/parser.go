package parser

import (
	"fmt"
	"strconv"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/token"
)

type ParserError struct {
	token   token.Token
	message string
}

func (e *ParserError) Error() string {
	if e.token.Type == token.EOF {
		return fmt.Sprintf("[line %d]: Error at end: %s", e.token.Line, e.message)
	}
	return fmt.Sprintf("[line %d]: Error at '%s': %s", e.token.Line, e.token.Lexeme, e.message)
}

type Parser struct {
	tokens []token.Token
	curr   int
}

func New(tokens []token.Token) *Parser {
	return &Parser{tokens: tokens}
}

func (p *Parser) Parse() (expr ast.Expr, err error) {
	defer func() {
		if r := recover(); r != nil {
			if e, ok := r.(*ParserError); ok {
				err = e
			} else {
				panic(r)
			}
		}
	}()

	return p.expression(), nil
}

func (p *Parser) expression() ast.Expr {
	return p.equality()
}

type rule func() ast.Expr

// Creates a left-associating binary expression if the next token matches any
// of the provided token types.
func (p *Parser) leftAssoc(next rule, types ...token.Type) ast.Expr {
	expr := next()

	for p.match(types...) {
		op := p.prev()
		right := next()
		expr = ast.BinaryExpr{
			Operator: op,
			Left:     expr,
			Right:    right,
		}
	}

	return expr
}

func (p *Parser) equality() ast.Expr {
	return p.leftAssoc(p.comparison, token.BangEqual, token.EqualEqual)
}

func (p *Parser) comparison() ast.Expr {
	return p.leftAssoc(
		p.term,
		token.Greater,
		token.GreaterEqual,
		token.Less,
		token.LessEqual,
	)
}

func (p *Parser) term() ast.Expr {
	return p.leftAssoc(p.factor, token.Minus, token.Plus)
}

func (p *Parser) factor() ast.Expr {
	return p.leftAssoc(p.unary, token.Slash, token.Star)
}

func (p *Parser) unary() ast.Expr {
	if p.match(token.Bang, token.Minus) {
		op := p.prev()
		expr := p.unary()
		return ast.UnaryExpr{
			Operator: op,
			Expr:     expr,
		}
	}

	return p.primary()
}

func (p *Parser) primary() ast.Expr {
	switch n := p.next(); n.Type {
	case token.False:
		return ast.NewLiteralExpr(false)
	case token.True:
		return ast.NewLiteralExpr(true)
	case token.Nil:
		return ast.NewLiteralExpr(nil)
	case token.Number:
		number, err := strconv.ParseFloat(n.Lexeme, 64)
		if err != nil {
			// Should be unreachable
			panic(err)
		}
		return ast.NewLiteralExpr(number)
	case token.String:
		return ast.NewLiteralExpr(n.Lexeme)
	case token.LeftParen:
		expr := p.expression()
		p.consume(token.RightParen, "expected ')' after expression")
		return ast.GroupingExpr{Expr: expr}
	default:
		panic(fmt.Sprint("unknown token ", n))
	}
}

func (p *Parser) consume(ttype token.Type, message string) token.Token {
	if p.check(ttype) {
		return p.next()
	}

	panic(&ParserError{p.peek(), message})
}

func (p *Parser) Synchronise() {
	p.next()

	for !p.done() {
		if p.prev().Type == token.Semicolon {
			return
		}

		switch p.peek().Type {
		case token.Class,
			token.Fun,
			token.Var,
			token.For,
			token.If,
			token.While,
			token.Print,
			token.Return:
			return
		}

		p.next()
	}
}

func (p *Parser) match(types ...token.Type) bool {
	for _, ttype := range types {
		if p.check(ttype) {
			p.next()
			return true
		}
	}

	return false
}

func (p *Parser) check(ttype token.Type) bool {
	if p.done() {
		return false
	}

	return p.peek().Type == ttype
}

func (p *Parser) done() bool {
	return p.peek().Type == token.EOF
}

func (p *Parser) next() token.Token {
	if !p.done() {
		p.curr++
	}

	return p.prev()
}

func (p *Parser) peek() token.Token {
	return p.tokens[p.curr]
}

func (p *Parser) prev() token.Token {
	return p.tokens[p.curr-1]
}

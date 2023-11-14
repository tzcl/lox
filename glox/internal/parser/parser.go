package parser

import (
	"fmt"
	"strconv"

	// TODO: Can I make these dot imports?
	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/token"
)

type Parser struct {
	tokens []token.Token
	curr   int
}

func New(tokens []token.Token) *Parser {
	return &Parser{tokens: tokens}
}

func (p *Parser) Parse() (ast.Expr, error) {
	defer func() {
		// if r := recover(); r != nil {
		// TODO: What do we need to do here?

		// }
	}()

	return p.expression(), nil
}

func (p *Parser) expression() ast.Expr {
	return p.equality()
}

func (p *Parser) equality() ast.Expr {
	expr := p.comparison()

	for p.match(token.BangEqual, token.EqualEqual) {
		op := p.prev()
		right := p.comparison()
		expr = ast.BinaryExpr{
			Operator: op,
			Left:     expr,
			Right:    right,
		}
	}

	return expr
}

// TODO: This and equality are almost identical
func (p *Parser) comparison() ast.Expr {
	expr := p.term()

	for p.match(
		token.Greater,
		token.GreaterEqual,
		token.Less,
		token.LessEqual,
	) {
		op := p.prev()
		right := p.term()
		expr = ast.BinaryExpr{
			Operator: op,
			Left:     expr,
			Right:    right,
		}
	}

	return expr
}

func (p *Parser) term() ast.Expr {
	expr := p.factor()

	for p.match(token.Minus, token.Plus) {
		op := p.prev()
		right := p.factor()
		expr = ast.BinaryExpr{
			Operator: op,
			Left:     expr,
			Right:    right,
		}
	}

	return expr
}

func (p *Parser) factor() ast.Expr {
	expr := p.unary()

	for p.match(token.Slash, token.Star) {
		op := p.prev()
		right := p.unary()
		expr = ast.BinaryExpr{
			Operator: op,
			Left:     expr,
			Right:    right,
		}
	}

	return expr
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
			panic("couldn't parse number: " + err.Error())
		}
		return ast.NewLiteralExpr(number)
	case token.String:
		return ast.NewLiteralExpr(n.Lexeme)
	case token.LeftParen:
		expr := p.expression()
		p.consume(token.RightParen, "expect ')' after expression")
		return ast.GroupingExpr{Expr: expr}
	default:
		panic(fmt.Sprint("unknown token ", n))
	}
}

func (p *Parser) consume(ttype token.Type, message string) token.Token {
	for !p.done() {
		if p.check(ttype) {
			return p.next()
		}
	}

	panic(message)
}

// func (p *Parser) synchronise() {
// TODO: implement
// }

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

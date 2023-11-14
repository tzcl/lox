package ast

import (
	"fmt"

	"github.com/tzcl/lox/glox/internal/token"
)

//sumtype:decl
type Expr interface {
	expr()
}

type LiteralExpr struct {
	Literal Literal
}

func NewLiteralExpr(literal any) LiteralExpr {
	switch value := literal.(type) {
	case nil:
		return LiteralExpr{Literal: NilLiteral{}}
	case bool:
		return LiteralExpr{Literal: BoolLiteral{value: value}}
	case int:
		return LiteralExpr{Literal: NumberLiteral{value: float64(value)}}
	case float64:
		return LiteralExpr{Literal: NumberLiteral{value: value}}
	case string:
		return LiteralExpr{Literal: StringLiteral{value: value}}
	default:
		panic(fmt.Sprintf("unknown literal %T", literal))
	}
}

func (LiteralExpr) expr() {}

type UnaryExpr struct {
	Expr     Expr
	Operator token.Token
}

func (UnaryExpr) expr() {}

type BinaryExpr struct {
	Left, Right Expr
	Operator    token.Token
}

func (BinaryExpr) expr() {}

type GroupingExpr struct {
	Expr Expr
}

func (GroupingExpr) expr() {}

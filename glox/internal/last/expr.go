package last

import "github.com/tzcl/lox/glox/internal/ltoken"

// TODO: Use sum type tool to check that all cases are being matched.
type Expr interface {
	expr()
}

type LiteralExpr struct {
	Value any
}

func (LiteralExpr) expr() {}

type UnaryExpr struct {
	Expr     Expr
	Operator ltoken.Token
}

func (UnaryExpr) expr() {}

type BinaryExpr struct {
	Left, Right Expr
	Operator    ltoken.Token
}

func (BinaryExpr) expr() {}

type GroupingExpr struct {
	Expr Expr
}

func (GroupingExpr) expr() {}

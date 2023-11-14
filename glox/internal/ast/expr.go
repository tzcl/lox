package ast

import "github.com/tzcl/lox/glox/internal/token"

//sumtype:decl
type Expr interface {
	expr()
}

type LiteralExpr struct {
	Value any
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

package ast

import (
	"github.com/tzcl/lox/glox/internal/token"
)

//sumtype:decl
type Node interface {
	node()
}

//sumtype:decl
type Stmt interface {
	Node
	stmt()
}

type PrintStmt struct {
	Expr Expr
}

func (PrintStmt) node() {}
func (PrintStmt) stmt() {}

type ExprStmt struct {
	Expr Expr
}

func (ExprStmt) node() {}
func (ExprStmt) stmt() {}

type VarStmt struct {
	Name        token.Token
	Initialiser Expr
}

func (VarStmt) node() {}
func (VarStmt) stmt() {}

type BlockStmt struct {
	Stmts []Stmt
}

func (BlockStmt) node() {}
func (BlockStmt) stmt() {}

//sumtype:decl
type Expr interface {
	Node
	expr()
}

type LiteralExpr struct {
	Literal any
}

func (LiteralExpr) node() {}
func (LiteralExpr) expr() {}

type UnaryExpr struct {
	Expr     Expr
	Operator token.Token
}

func (UnaryExpr) node() {}
func (UnaryExpr) expr() {}

type BinaryExpr struct {
	Left, Right Expr
	Operator    token.Token
}

func (BinaryExpr) node() {}
func (BinaryExpr) expr() {}

type GroupingExpr struct {
	Expr Expr
}

func (GroupingExpr) node() {}
func (GroupingExpr) expr() {}

type ConditionalExpr struct {
	Cond, Then, Alt Expr
}

func (ConditionalExpr) node() {}
func (ConditionalExpr) expr() {}

type VarExpr struct {
	Name token.Token
}

func (VarExpr) node() {}
func (VarExpr) expr() {}

type AssignExpr struct {
	Name  token.Token
	Value Expr
}

func (AssignExpr) node() {}
func (AssignExpr) expr() {}

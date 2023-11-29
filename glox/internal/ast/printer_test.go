package ast_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/token"
)

func TestPrint(t *testing.T) {
	expr := ast.BinaryExpr{
		Left: ast.UnaryExpr{
			Expr:     ast.LiteralExpr{123},
			Operator: token.Token{Type: token.Minus, Lexeme: "-", Line: 1},
		},
		Right: ast.GroupingExpr{
			Expr: ast.LiteralExpr{45.67},
		},
		Operator: token.Token{Type: token.Star, Lexeme: "*", Line: 1},
	}

	autogold.Expect("(* (- 123) (45.67))").Equal(t, ast.Print(expr))
}

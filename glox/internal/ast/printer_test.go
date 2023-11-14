package ast_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/token"
)

func TestPrint(t *testing.T) {
	expr := ast.BinaryExpr{
		Operator: token.Token{Type: token.Star, Lexeme: "*", Literal: nil, Line: 1},
		Left: ast.UnaryExpr{
			Operator: token.Token{Type: token.Minus, Lexeme: "-", Literal: nil, Line: 1},
			Expr:     ast.LiteralExpr{123},
		},
		Right: ast.GroupingExpr{
			Expr: ast.LiteralExpr{45.67},
		},
	}

	autogold.Expect("(* (- 123) (45.67))").Equal(t, ast.Print(expr))
}

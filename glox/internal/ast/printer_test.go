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

func TestPrintRPN(t *testing.T) {
	expr := ast.BinaryExpr{
		Left: ast.BinaryExpr{
			Left:     ast.LiteralExpr{1},
			Right:    ast.LiteralExpr{2},
			Operator: token.Token{Type: token.Plus, Lexeme: "+", Line: 1},
		},
		Right: ast.BinaryExpr{
			Left: ast.LiteralExpr{3},
			Right: ast.GroupingExpr{
				Expr: ast.BinaryExpr{
					Left:     ast.LiteralExpr{4},
					Right:    ast.LiteralExpr{5},
					Operator: token.Token{Type: token.Plus, Lexeme: "+", Line: 1},
				},
			},
			Operator: token.Token{Type: token.Star, Lexeme: "*", Line: 1},
		},
		Operator: token.Token{Type: token.Star, Lexeme: "*", Line: 1},
	}

	autogold.Expect("1 2 + 3 4 5 + * *").Equal(t, ast.PrintRPN(expr))
}

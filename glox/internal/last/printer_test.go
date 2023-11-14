package last_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/last"
	"github.com/tzcl/lox/glox/internal/ltoken"
)

func TestPrint(t *testing.T) {
	expr := last.BinaryExpr{
		Operator: ltoken.Token{Type: ltoken.Star, Lexeme: "*", Literal: nil, Line: 1},
		Left: last.UnaryExpr{
			Operator: ltoken.Token{Type: ltoken.Minus, Lexeme: "-", Literal: nil, Line: 1},
			Expr:     last.LiteralExpr{123},
		},
		Right: last.GroupingExpr{
			Expr: last.LiteralExpr{45.67},
		},
	}

	autogold.Expect("(* (- 123) (45.67))").Equal(t, last.Print(expr))
}

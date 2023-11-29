package interpreter

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func Test_evaluate(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"Maths": {
			source: "1+2*3/(4-5);",
			expect: autogold.Expect(float64(-5)),
		},
		"Equality": {
			source: "false == true == false;",
			expect: autogold.Expect(true),
		},
		"NilEquals": {
			source: "nil == nil;",
			expect: autogold.Expect(true),
		},
		"NilNotEquals": {
			source: "nil == true;",
			expect: autogold.Expect(false),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			scanner := scanner.New(test.source)
			tokens, err := scanner.Scan()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			parser := parser.New(tokens)
			stmts, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			expr, ok := stmts[0].(ast.ExprStmt)
			if !ok {
				t.Fatal("unexpected statement: ", stmts[0])
			}

			result, err := evaluate(expr.Expr)
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}
			test.expect.Equal(t, result)
		})
	}
}

func Test_evaluateError(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"InvalidOperands": {
			source: `1+"2";`,
			expect: autogold.Expect("[line 1]: Error at '+': Operands must be numbers"),
		},
		"DivideByZero": {
			source: "1/0;",
			expect: autogold.Expect("[line 1]: Error at '/': Dividing by zero"),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			scanner := scanner.New(test.source)
			tokens, err := scanner.Scan()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			parser := parser.New(tokens)
			stmts, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			expr, ok := stmts[0].(ast.ExprStmt)
			if !ok {
				t.Fatal("unexpected statement: ", stmts[0])
			}

			_, err = evaluate(expr.Expr)
			test.expect.Equal(t, err.Error())
		})
	}
}

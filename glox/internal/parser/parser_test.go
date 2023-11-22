package parser_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func TestParser_Parse(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"Success": {
			source: "1+2*3/(4-5)",
			expect: autogold.Expect("(+ 1 (/ (* 2 3) ((- 4 5))))"),
		},
		"Comma": {
			source: `"a", "b"`,
			expect: autogold.Expect(`(, "a" "b")`),
		},
		"Conditional": {
			source: `1 > 2 ? ":O" : ":D"`,
			expect: autogold.Expect(`(if (> 1 2) then ":O" else ":D")`),
		},
		"NestedConditional": {
			source: `1 > 2 ? "a" : 3 > 4 ? "b" : "c"`,
			expect: autogold.Expect(`(if (> 1 2) then "a" else (if (> 3 4) then "b" else "c"))`),
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
			expr, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			test.expect.Equal(t, ast.Print(expr))
		})
	}
}

func TestParser_ParseError(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"AtEnd": {
			source: "(1 + 1",
			expect: autogold.Expect("[line 1]: Error at end: expected ')' after expression"),
		},
		"AtToken": {
			source: "(1 + 1       a",
			expect: autogold.Expect("[line 1]: Error at 'Identifier(a)': expected ')' after expression"),
		},
		"Conditional": {
			source: "1 > 2 ? 3",
			expect: autogold.Expect("[line 1]: Error at end: Conditional expression missing ':'"),
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
			_, err = parser.Parse()
			if err == nil {
				t.Fatal("expected an error")
			}

			test.expect.Equal(t, err.Error())
		})
	}
}

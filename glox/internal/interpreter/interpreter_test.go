package interpreter_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/interpreter"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func TestInterpret(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"Maths": {
			source: "1+2*3/(4-5)",
			expect: autogold.Expect(float64(-5)),
		},
		"Equality": {
			source: "false == true == false",
			expect: autogold.Expect(true),
		},
		"NilEquals": {
			source: "nil == nil",
			expect: autogold.Expect(true),
		},
		"NilNotEquals": {
			source: "nil == true",
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
			expr, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			result, err := interpreter.Interpret(expr)
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}
			test.expect.Equal(t, result)
		})
	}
}

func TestInterpretError(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"InvalidOperands": {
			source: `1+"2"`,
			expect: autogold.Expect("[line 1]: Error at '+': Operands must be numbers"),
		},
		"DivideByZero": {
			source: "1/0",
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
			expr, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			_, err = interpreter.Interpret(expr)
			test.expect.Equal(t, err.Error())
		})
	}
}

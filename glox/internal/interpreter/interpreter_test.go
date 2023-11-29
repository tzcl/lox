package interpreter_test

import (
	"bytes"
	"os"
	"strings"
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/interpreter"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func TestInterpreter_Interpret(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"Variables": {
			source: `
					var a = 1;
					var b = 2;
					print a + b;
				`,
			expect: autogold.Expect("3"),
		},
		"Scope": {
			source: read("scope.lox"),
			expect: autogold.Expect(`inner a
outer b
global c
outer a
outer b
global c
global a
global b
global c`),
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

			var buffer bytes.Buffer

			interpreter := interpreter.New(&buffer)
			err = interpreter.Interpret(stmts)
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			got := strings.TrimSpace(buffer.String())
			test.expect.Equal(t, got)
		})
	}
}

func read(filename string) string {
	data, err := os.ReadFile("testdata/" + filename)
	if err != nil {
		panic(err)
	}

	return string(data)
}

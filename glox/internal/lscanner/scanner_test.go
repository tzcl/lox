package lscanner_test

import (
	"fmt"
	"testing"

	"github.com/hexops/autogold/v2"
	"github.com/tzcl/lox/glox/internal/lscanner"
)

func TestScan(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"RightBrace": {
			source: "}",
			expect: autogold.Expect("[RightBrace EOF]"),
		},
		"BangEqual": {
			source: "!=",
			expect: autogold.Expect("[BangEqual EOF]"),
		},
		"String": {
			source: `"string"`,
			expect: autogold.Expect(`[String("string") EOF]`),
		},
		"Number": {
			source: "4.123",
			expect: autogold.Expect("[Number(4.123) EOF]"),
		},
		"Identifier": {
			source: "test",
			expect: autogold.Expect("[Identifier(test) EOF]"),
		},
	}

	for name, test := range tests {
		t.Run(name, func(t *testing.T) {
			scanner := lscanner.New(test.source)
			tokens, err := scanner.Scan()
			if err != nil {
				t.Fatal("failed to scan text")
			}
			test.expect.Equal(t, fmt.Sprint(tokens))
		})
	}
}

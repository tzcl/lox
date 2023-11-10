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

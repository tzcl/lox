package scanner_test

import (
	"fmt"
	"strings"
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/scanner"
	"github.com/tzcl/lox/glox/internal/token"
)

func TestScan(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"RightBrace": {
			source: "}",
			expect: autogold.Expect("1 RightBrace → 1 EOF"),
		},
		"BangEqual": {
			source: "!=",
			expect: autogold.Expect("1 BangEqual → 1 EOF"),
		},
		"String": {
			source: `"string"`,
			expect: autogold.Expect(`1 String("string") → 1 EOF`),
		},
		"Number": {
			source: "4.123",
			expect: autogold.Expect("1 Number(4.123) → 1 EOF"),
		},
		"Identifier": {
			source: "test",
			expect: autogold.Expect("1 Identifier(test) → 1 EOF"),
		},
		"Comment": {
			source: `1234
				// This is a comment
				test`,
			expect: autogold.Expect("1 Number(1234) → 3 Identifier(test) → 3 EOF"),
		},
		"BlockComment": {
			source: `1234
				/* This is a multiline comment
			       that is very useful */
				test`,
			expect: autogold.Expect("1 Number(1234) → 4 Identifier(test) → 4 EOF"),
		},
		"NestedBlockComment": {
			source: `1234
				/* This is a 
				   /* nested */ 
				   block comment 
				*/
				test`,
			expect: autogold.Expect("1 Number(1234) → 6 Identifier(test) → 6 EOF"),
		},
	}

	for name, test := range tests {
		t.Run(name, func(t *testing.T) {
			scanner := scanner.New(test.source)
			tokens, err := scanner.Scan()
			if err != nil {
				t.Fatal("failed to scan text: ", err)
			}
			test.expect.Equal(t, formatTokens(tokens))
		})
	}
}

func TestScanError(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"UnterminatedString": {
			source: `"asdf`,
			expect: autogold.Expect(`[line 1]: Error: unterminated string: "asdf`),
		},
		"LongUnterminatedString": {
			source: `"asdf
jkl;
zxcv
bnm,`,
			expect: autogold.Expect(`[line 4]: Error: unterminated string: "asdf
jkl;
zxcv
bnm,`),
		},
		"UninitiatedString": {
			source: `asdf" 1234`,
			expect: autogold.Expect(`[line 1]: Error: string missing opening quote: asdf"`),
		},
		"UnterminatedBlockComment": {
			source: `/* asdf`,
			expect: autogold.Expect("[line 1]: Error: unterminated block comment"),
		},
		"UninitiatedBlockComment": {
			source: `asdf */`,
			expect: autogold.Expect("[line 1]: Error: found block comment without matching /*"),
		},
	}

	for name, test := range tests {
		t.Run(name, func(t *testing.T) {
			scanner := scanner.New(test.source)
			tokens, err := scanner.Scan()
			t.Log(tokens)
			if err == nil {
				t.Fatal("expected an error")
			}
			test.expect.Equal(t, err.Error())
		})
	}
}

func formatTokens(tokens []token.Token) string {
	var builder strings.Builder
	for i, token := range tokens {
		if i > 0 {
			builder.WriteString(" → ")
		}
		builder.WriteString(fmt.Sprintf("%d %v", token.Line, token))
	}

	return builder.String()
}

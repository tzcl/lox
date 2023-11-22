package token_test

import (
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/token"
)

func TestLookupKeyword(t *testing.T) {
	tests := map[string]struct {
		identifier string
		expect     autogold.Value
	}{
		"Found": {
			identifier: "return",
			expect:     autogold.Expect("return"),
		},
		"NotFound": {
			identifier: "not a keyword",
			expect:     autogold.Expect("Identifier"),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got := token.LookupKeyword(test.identifier)
			test.expect.Equal(t, got.String())
		})
	}
}

func TestToken_Debug(t *testing.T) {
	tests := map[string]struct {
		token  token.Token
		expect autogold.Value
	}{
		"EOF": {
			token:  token.Token{Type: token.EOF},
			expect: autogold.Expect("EOF"),
		},
		"String": {
			token:  token.Token{Type: token.String, Lexeme: "hello world :D"},
			expect: autogold.Expect(`String("hello world :D")`),
		},
		"Identifier": {
			token:  token.Token{Type: token.Identifier, Lexeme: "_arst_123"},
			expect: autogold.Expect("Identifier(_arst_123)"),
		},
		"Number": {
			token:  token.Token{Type: token.Number, Lexeme: "121.835"},
			expect: autogold.Expect("Number(121.835)"),
		},
		"RightBrace": {
			token:  token.Token{Type: token.RightBrace},
			expect: autogold.Expect("RightBrace"),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got := test.token.Debug()
			test.expect.Equal(t, got)
		})
	}
}

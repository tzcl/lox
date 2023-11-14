package ltoken_test

import (
	"github.com/hexops/autogold/v2"
	"testing"

	"github.com/tzcl/lox/glox/internal/ltoken"
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
			got := ltoken.LookupKeyword(test.identifier)
			test.expect.Equal(t, got.String())
		})
	}
}

func TestToken_String(t *testing.T) {
	tests := map[string]struct {
		token  ltoken.Token
		expect autogold.Value
	}{
		"EOF": {
			token:  ltoken.Token{Type: ltoken.EOF},
			expect: autogold.Expect("EOF"),
		},
		"String": {
			token:  ltoken.Token{Type: ltoken.String, Literal: "hello world :D"},
			expect: autogold.Expect(`String("hello world :D")`),
		},
		"Identifier": {
			token:  ltoken.Token{Type: ltoken.Identifier, Literal: "_arst_123"},
			expect: autogold.Expect("Identifier(_arst_123)"),
		},
		"Number": {
			token:  ltoken.Token{Type: ltoken.Number, Literal: 121.835},
			expect: autogold.Expect("Number(121.835)"),
		},
		"RightBrace": {
			token:  ltoken.Token{Type: ltoken.RightBrace},
			expect: autogold.Expect("RightBrace"),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got := test.token.String()
			test.expect.Equal(t, got)
		})
	}
}

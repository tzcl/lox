package token

import "fmt"

type Type int

const (
	// Single-character tokens
	LeftParen Type = iota
	RightParen
	LeftBrace
	RightBrace
	Comma
	Dot
	Minus
	Plus
	Semicolon
	Slash
	Star

	// One or two character tokens
	Bang
	BangEqual
	Equal
	EqualEqual
	Greater
	GreaterEqual
	Less
	LessEqual

	// Literals
	Identifier
	String
	Number

	// Keywords
	And
	Class
	Else
	False
	Fun
	For
	If
	Nil
	Or
	Print
	Return
	Super
	This
	True
	Var
	While

	EOF
)

type Token struct {
	Type    Type
	Lexeme  string
	Literal any // TODO: How to represent this?
	Lint    int
}

func (t Token) String() string {
	// TODO: Literal...
	return fmt.Sprintf("%d %s", t.Type, t.Lexeme) /* + " " + string(t.Literal) */
}

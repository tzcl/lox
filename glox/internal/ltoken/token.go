package ltoken

import "fmt"

//go:generate stringer -type=Type
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

var keywords = map[string]Type{
	"and":    And,
	"class":  Class,
	"else":   Else,
	"false":  False,
	"for":    For,
	"fun":    Fun,
	"if":     If,
	"nil":    Nil,
	"or":     Or,
	"print":  Print,
	"return": Return,
	"super":  Super,
	"this":   This,
	"true":   True,
	"var":    Var,
	"while":  While,
}

func LookupKeyword(identifier string) Type {
	ttype, ok := keywords[identifier]
	if !ok {
		return Identifier
	}
	return ttype
}

type Token struct {
	Type    Type
	Lexeme  string
	Literal any // TODO: How to represent this?
	Line    int
}

func (t Token) String() string {
	switch t.Type {
	case EOF:
		return "EOF"
	case String:
		return fmt.Sprintf("%s(%q)", t.Type, t.Literal)
	case Identifier, Number:
		return fmt.Sprintf("%s(%v)", t.Type, t.Literal)
	default:
		return fmt.Sprintf("%s", t.Type)
	}
}

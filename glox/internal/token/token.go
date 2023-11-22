package token

import (
	"fmt"
)

//go:generate stringer -type=Type -linecomment
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
	QuestionMark
	Colon

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
	keywordStart
	And    // and
	Class  // class
	Else   // else
	False  // false
	Fun    // fun
	For    // for
	If     // if
	Nil    // nil
	Or     // or
	Print  // print
	Return // return
	Super  // super
	This   // this
	True   // true
	Var    // var
	While  // while
	keywordEnd

	EOF
	numTokens
)

var keywords = map[string]Type{}

func init() {
	// Initialise keywords
	for i := keywordStart + 1; i < keywordEnd; i++ {
		keywords[i.String()] = i
	}
}

func LookupKeyword(identifier string) Type {
	if ttype, ok := keywords[identifier]; ok {
		return ttype
	}
	return Identifier
}

type Token struct {
	Type   Type
	Lexeme string
	Line   int
}

// Debug converts the token into a string useful for debugging.
func (t Token) Debug() string {
	switch ty := t.Type; ty {
	case EOF:
		return "EOF"
	case String:
		return fmt.Sprintf("%s(%q)", ty, t.Lexeme)
	case Identifier, Number:
		return fmt.Sprintf("%s(%v)", ty, t.Lexeme)
	default:
		return ty.String()
	}
}

// String converts the token to a string useful for the user.
func (t Token) String() string {
	switch t.Type {
	case EOF:
		return "EOF"
	default:
		return t.Lexeme
	}
}

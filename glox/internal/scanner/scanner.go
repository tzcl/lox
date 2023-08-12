package scanner

import "github.com/tzcl/lox/glox/internal/token"

type Scanner struct {
	source string
	tokens []token.Token
}

func New(source string) *Scanner {
	return &Scanner{source: source}
}

// TODO: What else does this function need?
func (s *Scanner) Scan() {

}

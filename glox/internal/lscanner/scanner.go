package lscanner

import (
	"fmt"
	"strconv"
	"unicode"

	"github.com/tzcl/lox/glox/internal/ltoken"
)

type ScannerError struct {
	line    int
	message string
}

func (e *ScannerError) Error() string {
	return fmt.Sprintf("[line %d]: Error: %s", e.line, e.message)
}

type Scanner struct {
	// Storing the source as a slice of runes allows us to handle Unicode
	// characters
	source []rune

	start int
	curr  int
	line  int

	tokens []ltoken.Token
}

func New(source string) *Scanner {
	return &Scanner{source: []rune(source), line: 1}
}

func (s *Scanner) Scan() ([]ltoken.Token, error) {
	for !s.done() {
		if err := s.scanToken(); err != nil {
			return nil, err
		}
	}

	s.tokens = append(s.tokens, ltoken.Token{Type: ltoken.EOF, Line: s.line})
	return s.tokens, nil
}

func (s *Scanner) scanToken() error {
	s.start = s.curr

	switch r := s.next(); r {
	// Single-character tokens
	case '(':
		s.addToken(ltoken.LeftParen, nil)
	case ')':
		s.addToken(ltoken.RightParen, nil)
	case '{':
		s.addToken(ltoken.LeftBrace, nil)
	case '}':
		s.addToken(ltoken.RightBrace, nil)
	case ',':
		s.addToken(ltoken.Comma, nil)
	case '.':
		s.addToken(ltoken.Dot, nil)
	case '-':
		s.addToken(ltoken.Minus, nil)
	case '+':
		s.addToken(ltoken.Plus, nil)
	case ';':
		s.addToken(ltoken.Semicolon, nil)

	// Double-character tokens
	case '!':
		ttype := ltoken.Bang
		if s.match('=') {
			ttype = ltoken.BangEqual
		}
		s.addToken(ttype, nil)
	case '=':
		ttype := ltoken.Equal
		if s.match('=') {
			ttype = ltoken.EqualEqual
		}
		s.addToken(ttype, nil)
	case '<':
		ttype := ltoken.Less
		if s.match('=') {
			ttype = ltoken.LessEqual
		}
		s.addToken(ttype, nil)
	case '>':
		ttype := ltoken.Greater
		if s.match('=') {
			ttype = ltoken.GreaterEqual
		}
		s.addToken(ttype, nil)
	case '/':
		switch n := s.peek(); n {
		case '/':
			s.next() // Consume the '/'
			for s.peek() != '\n' && !s.done() {
				s.next()
			}
		case '*':
			s.next() // Consume the '*'
			if err := s.blockComment(); err != nil {
				return err
			}
		default:
			s.addToken(ltoken.Slash, nil)
		}
	case '*':
		if s.peek() == '/' {
			return s.error("found block comment without matching /*")
		}
		s.addToken(ltoken.Star, nil)

	// Ignore whitespace
	case ' ', '\r', '\t':
	case '\n':
		s.line++

	// Parse string
	case '"':
		if err := s.string(); err != nil {
			return err
		}

	// Parse more complex tokens
	default:
		switch {
		case unicode.IsDigit(r):
			s.number()
		case isLetter(r):
			if err := s.identifier(); err != nil {
				return err
			}
		default:
			return s.error("unexpected character")
		}
	}

	return nil
}

func (s *Scanner) string() error {
	// Find end of string
	for s.peek() != '"' && !s.done() {
		if s.peek() == '\n' {
			s.line++
		}
		s.next()
	}

	if s.done() {
		return s.error("unterminated string: " + string(s.source[s.start:s.curr]))
	}

	// The closing "
	s.next()

	// Trim the surrounding quotes
	value := string(s.source[s.start+1 : s.curr-1])
	s.addToken(ltoken.String, value)

	return nil
}

func (s *Scanner) number() {
	// Find end of number
	for unicode.IsDigit(s.peek()) {
		s.next()
	}

	// Look for fractional part
	if s.peek() == '.' && unicode.IsDigit(s.peekNext()) {
		// Consume the "."
		s.next()

		// Find end of fractional part
		for unicode.IsDigit(s.peek()) {
			s.next()
		}
	}

	// Ignore error because we've checked we have a valid float
	value, _ := strconv.ParseFloat(string(s.source[s.start:s.curr]), 64)
	s.addToken(ltoken.Number, value)
}

func (s *Scanner) identifier() error {
	// Find end of identifier
	for r := s.peek(); isLetter(r) || unicode.IsDigit(r); r = s.peek() {
		s.next()
	}

	if s.match('"') {
		return s.error(`string missing opening quote: ` + string(s.source[s.start:s.curr]))
	}

	ident := string(s.source[s.start:s.curr])
	ttype := ltoken.LookupKeyword(ident)
	s.addToken(ttype, ident)

	return nil
}

func (s *Scanner) blockComment() error {
	var depth int

	for !s.done() {
		switch {
		case s.match('/') && s.match('*'):
			depth++
		case s.match('*') && s.match('/'):
			depth--
			if depth < 0 {
				return nil
			}
		case s.match('\n'):
			s.line++
		default:
			s.next()
		}
	}

	return s.error("unterminated block comment")
}

func (s *Scanner) done() bool {
	return s.curr >= len(s.source)
}

func (s *Scanner) match(r rune) bool {
	if s.done() {
		return false
	}
	if s.source[s.curr] != r {
		return false
	}

	s.curr++
	return true
}

func (s *Scanner) next() rune {
	r := s.source[s.curr]
	s.curr++
	return r
}

func (s *Scanner) peek() rune {
	if s.done() {
		// rune(0) == '\000' == '\x00' == '\u0000'
		return rune(0)
	}

	return s.source[s.curr]
}

func (s *Scanner) peekNext() rune {
	if s.curr+1 >= len(s.source) {
		return rune(0)
	}

	return s.source[s.curr+1]
}

func (s *Scanner) addToken(ttype ltoken.Type, literal any) {
	lexeme := string(s.source[s.start:s.curr])
	token := ltoken.Token{Type: ttype, Lexeme: lexeme, Literal: literal, Line: s.line}
	s.tokens = append(s.tokens, token)
}

func (s *Scanner) error(message string) error {
	return &ScannerError{line: s.line, message: message}
}

// A letter can be a unicode letter or "_"
func isLetter(r rune) bool {
	return unicode.IsLetter(r) || r == '_'
}

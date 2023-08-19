package lscanner

import (
	"strconv"
	"unicode"

	"github.com/tzcl/lox/glox/internal/ltoken"
)

type Scanner struct {
	// Storing the source as a slice of runes allows us to handle Unicode
	// characters
	source []rune

	start int
	curr  int
	line  int

	tokens []ltoken.Token
}

// TODO: Error reporter?
func New(source string) *Scanner {
	return &Scanner{source: []rune(source), line: 1}
}

func (s *Scanner) Scan() []ltoken.Token {
	for !s.done() {
		s.scanToken()
	}

	s.tokens = append(s.tokens, ltoken.Token{Type: ltoken.EOF, Line: s.line})
	return s.tokens
}

func (s *Scanner) scanToken() {
	s.start = s.curr

	switch r := s.next(); r {
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
	case '*':
		s.addToken(ltoken.Star, nil)

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
		if s.match('/') {
			// Consume comment
			for s.peek() != '\n' && !s.done() {
				s.next()
			}
		} else {
			s.addToken(ltoken.Slash, nil)
		}

	case ' ', '\r', '\t':
	case '\n':
		s.line++

	case '"':
		s.string()

	default:
			switch {
				case unicode.IsDigit(r):
				s.number()
				case unicode.IsLetter(r):
				s.identifier()
			}
		if unicode.IsDigit(r) {
			s.number()
		} else {
			panic("unexpected charcter")
		}
	}
}

func (s *Scanner) string() {
	// Find end of string
	for s.peek() != '"' && !s.done() {
		if s.peek() == '\n' {
			s.line++
		}
		s.next()
	}

	if s.done() {
		panic("unterminated string")
	}

	// The closing "
	s.next()

	// Trim the surrounding quotes
	value := string(s.source[s.start+1 : s.curr-1])
	s.addToken(ltoken.String, value)
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

func (s *Scanner) identifier() {
	// Find end of identifier
	for unicode.IsLetter() || unicode.Is
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

// A letter can be a unicode letter or "_"
func isLetter(r rune) bool {
	return unicode.IsLetter(r) || r == '_'
}

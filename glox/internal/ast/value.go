package ast

import (
	"fmt"
	"strconv"
)

//sumtype:decl
type Literal interface {
	String() string
	literal()
}

type NilLiteral struct{}

func (NilLiteral) String() string {
	return "nil"
}

func (NilLiteral) literal() {}

type BoolLiteral struct {
	value bool
}

func (b BoolLiteral) String() string {
	return strconv.FormatBool(b.value)
}

func (BoolLiteral) literal() {}

type StringLiteral struct {
	value string
}

func (s StringLiteral) String() string {
	return fmt.Sprintf("%q", s.value)
}

func (StringLiteral) literal() {}

type NumberLiteral struct {
	value float64
}

func (n NumberLiteral) String() string {
	return strconv.FormatFloat(n.value, 'f', -1, 64)
}

func (NumberLiteral) literal() {}

package env

import "github.com/tzcl/lox/glox/internal/token"

type Environment struct {
	parent *Environment
	values map[string]any
}

func New(parent *Environment) *Environment {
	return &Environment{values: make(map[string]any), parent: parent}
}

func (e *Environment) Define(name string, value any) {
	e.values[name] = value
}

func (e *Environment) Get(name token.Token) any {
	if value, ok := e.values[name.Lexeme]; ok {
		return value
	}

	if e.parent != nil {
		return e.parent.Get(name)
	}

	// TODO: Throw error
	panic("undefined variable: " + name.UserString())
}

func (e *Environment) Assign(name token.Token, value any) {
	if _, ok := e.values[name.Lexeme]; ok {
		e.values[name.Lexeme] = value
		return
	}

	if e.parent != nil {
		e.parent.Assign(name, value)
		return
	}

	panic("undefined variable: " + name.UserString())
}

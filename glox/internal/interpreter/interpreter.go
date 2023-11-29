package interpreter

import (
	"fmt"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/token"
)

type Value any

// TODO: This is identical to ParserError
type InterpreterError struct {
	token   token.Token
	message string
}

func (e *InterpreterError) Error() string {
	if e.token.Type == token.EOF {
		return fmt.Sprintf("[line %d]: Error at end: %s", e.token.Line, e.message)
	}
	return fmt.Sprintf("[line %d]: Error at '%s': %s", e.token.Line, e.token.UserString(), e.message)
}

func Interpret(expr ast.Expr) (Value, error) {
	switch expr := expr.(type) {
	case ast.LiteralExpr:
		return literal(expr)
	case ast.GroupingExpr:
		return grouping(expr)
	case ast.UnaryExpr:
		return unary(expr)
	case ast.BinaryExpr:
		return binary(expr)
	default:
		panic(fmt.Sprintf("interpret: unknown expression type %T", expr))
	}
}

func literal(expr ast.LiteralExpr) (Value, error) {
	return expr.Literal, nil
}

func grouping(expr ast.GroupingExpr) (Value, error) {
	return Interpret(expr.Expr)
}

func unary(expr ast.UnaryExpr) (Value, error) {
	right, err := Interpret(expr.Expr)
	if err != nil {
		return nil, err
	}

	switch expr.Operator.Type {
	case token.Bang:
		return !isTruthy(right), nil
	case token.Minus:
		right, ok := right.(float64)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operand must be a number"}
		}
		return right, nil
	default:
		panic(fmt.Sprintf("interpret: unary expr: unknown operator: %T", expr.Operator.Type))
	}
}

func binary(expr ast.BinaryExpr) (Value, error) {
	left, err := Interpret(expr.Left)
	if err != nil {
		return nil, err
	}
	right, err := Interpret(expr.Right)
	if err != nil {
		return nil, err
	}

	switch expr.Operator.Type {
	case token.BangEqual:
		return left != right, nil
	case token.EqualEqual:
		return left == right, nil
	case token.Greater:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left > right, nil
	case token.GreaterEqual:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left >= right, nil
	case token.Less:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left < right, nil
	case token.LessEqual:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left <= right, nil
	case token.Minus:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left - right, nil
	case token.Plus:
		// I think operator overloading introduces unwarranted complexity,
		// especially since string concatenation is nothing like addition.
		// TODO: Add another token for string concatenation
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left + right, nil
	case token.Slash:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		if right == 0 {
			return nil, &InterpreterError{token: expr.Operator, message: "Dividing by zero"}
		}
		return left / right, nil
	case token.Star:
		left, right, ok := assertNumbers(left, right)
		if !ok {
			return nil, &InterpreterError{token: expr.Operator, message: "Operands must be numbers"}
		}
		return left * right, nil
	default:
		panic("interpret: binary expr: unreachable")
	}
}

func assertNumbers(left, right Value) (float64, float64, bool) {
	l, ok := left.(float64)
	if !ok {
		return 0, 0, false
	}
	r, ok := right.(float64)
	if !ok {
		return 0, 0, false
	}
	return l, r, true
}

func isTruthy(value Value) bool {
	switch value := value.(type) {
	case nil:
		return false
	case bool:
		return value
	default:
		return true
	}
}

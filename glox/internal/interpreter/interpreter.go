package interpreter

import (
	"fmt"
	"io"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/env"
	"github.com/tzcl/lox/glox/internal/token"
)

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

type Interpreter struct {
	env    *env.Environment
	writer io.StringWriter
}

func New(writer io.StringWriter) *Interpreter {
	// Every interpreter has its own global environment
	return &Interpreter{env: env.New(nil), writer: writer}
}

func (i *Interpreter) Interpret(stmts []ast.Stmt) error {
	for _, stmt := range stmts {
		if err := i.execute(stmt); err != nil {
			return err
		}
	}

	return nil
}

func (i *Interpreter) execute(stmt ast.Stmt) error {
	switch stmt := stmt.(type) {
	case ast.PrintStmt:
		value, err := i.evaluate(stmt.Expr)
		if err != nil {
			return err
		}
		_, err = i.writer.WriteString(fmt.Sprint(value) + "\n")
		if err != nil {
			return err
		}
	case ast.ExprStmt:
		if _, err := i.evaluate(stmt.Expr); err != nil {
			return err
		}
	case ast.VarStmt:
		var value any
		if stmt.Initialiser != nil {
			result, err := i.evaluate(stmt.Initialiser)
			if err != nil {
				return err
			}
			value = result
		}

		i.env.Define(stmt.Name.Lexeme, value)
	case ast.BlockStmt:
		if err := i.executeBlock(stmt.Stmts, env.New(i.env)); err != nil {
			return err
		}
	default:
		panic(fmt.Sprintf("interpret: unknown statement %T", stmt))
	}

	return nil
}

// TODO: Can I get rid of the env field?
func (i *Interpreter) executeBlock(stmts []ast.Stmt, env *env.Environment) error {
	prev := i.env
	defer func() {
		// Reset environment
		i.env = prev
	}()

	i.env = env

	for _, stmt := range stmts {
		if err := i.execute(stmt); err != nil {
			return err
		}
	}

	return nil
}

func (i *Interpreter) evaluate(expr ast.Expr) (any, error) {
	switch expr := expr.(type) {
	case ast.LiteralExpr:
		return literal(expr)
	case ast.GroupingExpr:
		return i.grouping(expr)
	case ast.UnaryExpr:
		return i.unary(expr)
	case ast.BinaryExpr:
		return i.binary(expr)
	case ast.VarExpr:
		return i.env.Get(expr.Name), nil
	case ast.AssignExpr:
		value, err := i.evaluate(expr.Value)
		if err != nil {
			return nil, err
		}
		i.env.Assign(expr.Name, value)
		return value, nil
	default:
		panic(fmt.Sprintf("interpret: unknown expression type %T", expr))
	}
}

func literal(expr ast.LiteralExpr) (any, error) {
	return expr.Literal, nil
}

func (i *Interpreter) grouping(expr ast.GroupingExpr) (any, error) {
	return i.evaluate(expr.Expr)
}

func (i *Interpreter) unary(expr ast.UnaryExpr) (any, error) {
	right, err := i.evaluate(expr.Expr)
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

func (i *Interpreter) binary(expr ast.BinaryExpr) (any, error) {
	left, err := i.evaluate(expr.Left)
	if err != nil {
		return nil, err
	}
	right, err := i.evaluate(expr.Right)
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

func assertNumbers(left, right any) (float64, float64, bool) {
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

func isTruthy(value any) bool {
	switch value := value.(type) {
	case nil:
		return false
	case bool:
		return value
	default:
		return true
	}
}

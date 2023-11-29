package ast

import (
	"fmt"
	"strings"
)

func Print(node Node) string {
	var builder strings.Builder

	switch node := node.(type) {
	case PrintStmt:
		builder.WriteString(fmt.Sprintf("(print %s)", Print(node.Expr)))
	case ExprStmt:
		builder.WriteString(Print(node.Expr))
	case LiteralExpr:
		builder.WriteString(printLiteral(node.Literal))
	case UnaryExpr:
		builder.WriteString(fmt.Sprintf("(%s %s)",
			node.Operator.Lexeme,
			Print(node.Expr),
		))
	case BinaryExpr:
		builder.WriteString(fmt.Sprintf("(%s %s %s)",
			node.Operator.Lexeme,
			Print(node.Left),
			Print(node.Right),
		))
	case GroupingExpr:
		builder.WriteString(fmt.Sprintf("(%s)",
			Print(node.Expr),
		))
	case ConditionalExpr:
		builder.WriteString(fmt.Sprintf("(if %s then %s else %s)",
			Print(node.Cond),
			Print(node.Then),
			Print(node.Alt),
		))
	case Stmt, Expr:
		return Print(node)
	default:
		panic(fmt.Sprintf("printer: unknown node %T", node))
	}

	return builder.String()
}

func printLiteral(lit any) string {
	switch lit := lit.(type) {
	case nil:
		return "nil"
	case string:
		return fmt.Sprintf("%q", lit)
	default:
		return fmt.Sprint(lit)
	}
}

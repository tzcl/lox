package ast

import (
	"fmt"
	"strings"
)

func Print(expr Expr) string {
	var builder strings.Builder

	switch expr := expr.(type) {
	case LiteralExpr:
		builder.WriteString(fmt.Sprint(expr.Value))
	case UnaryExpr:
		builder.WriteString(fmt.Sprintf(
			"(%s %s)",
			expr.Operator.Lexeme,
			Print(expr.Expr),
		))
	case BinaryExpr:
		builder.WriteString(fmt.Sprintf(
			"(%s %s %s)",
			expr.Operator.Lexeme,
			Print(expr.Left),
			Print(expr.Right),
		))
	case GroupingExpr:
		builder.WriteString(fmt.Sprintf(
			"(%s)",
			Print(expr.Expr),
		))
	}

	return builder.String()
}

func PrintRPN(expr Expr) string {
	var builder strings.Builder

	switch expr := expr.(type) {
	case LiteralExpr:
		builder.WriteString(fmt.Sprint(expr.Value))
	case UnaryExpr:
		builder.WriteString(fmt.Sprintf(
			"%s %s",
			PrintRPN(expr.Expr),
			expr.Operator.Lexeme,
		))
	case BinaryExpr:
		builder.WriteString(fmt.Sprintf(
			"%s %s %s",
			PrintRPN(expr.Left),
			PrintRPN(expr.Right),
			expr.Operator.Lexeme,
		))
	case GroupingExpr:
		builder.WriteString(PrintRPN(expr.Expr))
	}

	return builder.String()
}

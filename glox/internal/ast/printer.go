package ast

import (
	"fmt"
	"strings"
)

const indent = ". "

type visitor struct {
	builder *strings.Builder
	depth   int
}

func PrintProgram(stmts []Stmt) string {
	var builder strings.Builder

	for _, stmt := range stmts {
		builder.WriteString(Print(stmt))
	}

	return builder.String()
}

func Print(node Node) string {
	var builder strings.Builder
	v := visitor{builder: &builder}

	visit(node, v)

	return builder.String()
}

func visit(node Node, v visitor) {
	writeIndent := func() {
		for i := v.depth; i > 0; i-- {
			v.builder.WriteString(indent)
		}
	}

	switch node := node.(type) {
	case PrintStmt:
		writeIndent()
		v.builder.WriteString("(print ")
		visit(node.Expr, v)
		v.builder.WriteString(")\n")
	case ExprStmt:
		writeIndent()
		visit(node.Expr, v)
		v.builder.WriteString("\n")
	case VarStmt:
		writeIndent()
		v.builder.WriteString("(def " + node.Name.UserString() + " ")
		visit(node.Initialiser, v)
		v.builder.WriteString(")\n")
	case BlockStmt:
		writeIndent()
		v.builder.WriteString("{\n")
		for _, stmt := range node.Stmts {
			visit(stmt, visitor{v.builder, v.depth + 1})
		}
		writeIndent()
		v.builder.WriteString("}\n")
	case LiteralExpr:
		v.builder.WriteString(printLiteral(node.Literal))
	case UnaryExpr:
		v.builder.WriteString("(" + node.Operator.Lexeme + " ")
		visit(node.Expr, v)
		v.builder.WriteString(")")
	case BinaryExpr:
		v.builder.WriteString("(" + node.Operator.Lexeme + " ")
		visit(node.Left, v)
		v.builder.WriteString(" ")
		visit(node.Right, v)
		v.builder.WriteString(")")
	case GroupingExpr:
		v.builder.WriteString("(")
		visit(node.Expr, v)
		v.builder.WriteString(")")
	case ConditionalExpr:
		v.builder.WriteString("(if ")
		visit(node.Cond, v)
		v.builder.WriteString(" then ")
		visit(node.Then, v)
		v.builder.WriteString(" else ")
		visit(node.Alt, v)
		v.builder.WriteString(")")
	case VarExpr:
		v.builder.WriteString(node.Name.UserString())
	case AssignExpr:
		v.builder.WriteString("(set " + node.Name.UserString() + " ")
		visit(node.Value, v)
		v.builder.WriteString(")")
	case Stmt:
		panic(fmt.Sprintf("printer: unknown stmt %T", node))
	case Expr:
		panic(fmt.Sprintf("printer: unknown expr %T", node))
	default:
		panic(fmt.Sprintf("printer: unknown node %T", node))
	}
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

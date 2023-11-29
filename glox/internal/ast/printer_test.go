package ast_test

import (
	"fmt"
	"os"
	"strings"
	"testing"

	"github.com/hexops/autogold/v2"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func TestPrint(t *testing.T) {
	tests := map[string]struct {
		source string
		expect autogold.Value
	}{
		"Maths": {
			source: "-123 * (45.67);",
			expect: autogold.Expect("(* (- 123) (45.67))"),
		},
		"Block": {
			source: "var a = 1; { a = 2; var b = 4; }",
			expect: autogold.Expect(`(def a 1)
{
. (set a 2)
. (def b 4)
}`),
		},
	}

	t.Parallel()
	for name, test := range tests {
		test := test
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			scanner := scanner.New(test.source)
			tokens, err := scanner.Scan()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			parser := parser.New(tokens)
			stmts, err := parser.Parse()
			if err != nil {
				t.Fatal("unexpected err: ", err)
			}

			var builder strings.Builder
			for _, stmt := range stmts {
				builder.WriteString(ast.Print(stmt))
			}

			test.expect.Equal(t, strings.TrimSpace(builder.String()))
		})
	}
}

func Benchmark_Print(b *testing.B) {
	data, err := os.ReadFile("testdata/strings.lox")
	if err != nil {
		b.Fatal("unexpected err: ", err)
	}

	scanner := scanner.New(string(data))
	tokens, err := scanner.Scan()
	if err != nil {
		b.Fatal("unexpected err: ", err)
	}

	parser := parser.New(tokens)
	stmts, err := parser.Parse()
	if err != nil {
		b.Fatal("unexpected err: ", err)
	}

	b.ResetTimer()

	for n := 0; n < b.N; n++ {
		ast.PrintProgram(stmts)
	}
}

func Benchmark_PrintOld(b *testing.B) {
	data, err := os.ReadFile("testdata/strings.lox")
	if err != nil {
		b.Fatal("unexpected err: ", err)
	}

	scanner := scanner.New(string(data))
	tokens, err := scanner.Scan()
	if err != nil {
		b.Fatal("unexpected err: ", err)
	}

	parser := parser.New(tokens)
	stmts, err := parser.Parse()
	if err != nil {
		b.Fatal("unexpected err: ", err)

	}

	b.ResetTimer()

	for n := 0; n < b.N; n++ {
		printProgram(stmts)
	}
}

func printProgram(stmts []ast.Stmt) string {
	var builder strings.Builder

	for _, stmt := range stmts {
		builder.WriteString(print(stmt, 0))
	}

	return builder.String()
}

// print is the old implementation of print
func print(node ast.Node, depth int) string {
	var builder strings.Builder

	writeIndent := func() {
		for i := depth; i > 0; i-- {
			builder.WriteString(". ")
		}
	}

	switch node := node.(type) {
	case ast.PrintStmt:
		writeIndent()
		builder.WriteString(fmt.Sprintf("(print %s)\n", print(node.Expr, depth)))
	case ast.ExprStmt:
		writeIndent()
		builder.WriteString(print(node.Expr, depth) + "\n")
	case ast.VarStmt:
		writeIndent()
		builder.WriteString(fmt.Sprintf("(def %s %s)\n",
			node.Name.UserString(),
			print(node.Initialiser, depth),
		))
	case ast.BlockStmt:
		writeIndent()
		builder.WriteString("{\n")
		for _, stmt := range node.Stmts {
			print(stmt, depth+1)
		}
		writeIndent()
		builder.WriteString("}\n")
	case ast.LiteralExpr:
		builder.WriteString(printLiteral(node.Literal))
	case ast.UnaryExpr:
		builder.WriteString(fmt.Sprintf("(%s %s)",
			node.Operator.Lexeme,
			print(node.Expr, depth),
		))
	case ast.BinaryExpr:
		builder.WriteString(fmt.Sprintf("(%s %s %s)",
			node.Operator.Lexeme,
			print(node.Left, depth),
			print(node.Right, depth),
		))
	case ast.GroupingExpr:
		builder.WriteString(fmt.Sprintf("(%s)",
			print(node.Expr, depth),
		))
	case ast.ConditionalExpr:
		builder.WriteString(fmt.Sprintf("(if %s then %s else %s)",
			print(node.Cond, depth),
			print(node.Then, depth),
			print(node.Alt, depth),
		))
	case ast.VarExpr:
		builder.WriteString(node.Name.UserString())
	case ast.AssignExpr:
		builder.WriteString(fmt.Sprintf("(set %s %s)",
			node.Name.UserString(),
			print(node.Value, depth),
		))
	case ast.Stmt:
		panic(fmt.Sprintf("printer: unknown stmt %T", node))
	case ast.Expr:
		panic(fmt.Sprintf("printer: unknown expr %T", node))
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

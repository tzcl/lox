package main

import (
	"bufio"
	"fmt"
	"os"

	"github.com/tzcl/lox/glox/internal/ast"
	"github.com/tzcl/lox/glox/internal/interpreter"
	"github.com/tzcl/lox/glox/internal/parser"
	"github.com/tzcl/lox/glox/internal/scanner"
)

func main() {
	args := os.Args[1:]

	if len(args) > 1 {
		fmt.Println("Usage: lox [script]")
		os.Exit(1)
	} else if len(args) == 1 {
		runFile(args[0])
	} else {
		runPrompt()
	}
}

func runFile(path string) {
	data, err := os.ReadFile(path)
	if err != nil {
		fmt.Println(err)
		return
	}

	if err := run(string(data)); err != nil {
		fmt.Println(err)
	}
}

func runPrompt() {
	reader := bufio.NewScanner(os.Stdin)

	for {
		fmt.Print("> ")
		if ok := reader.Scan(); !ok {
			break
		}

		line := reader.Text()

		if err := run(line); err != nil {
			fmt.Println(err)
		}
	}
}

func run(src string) error {
	scanner := scanner.New(src)

	tokens, err := scanner.Scan()
	if err != nil {
		return err
	}

	fmt.Println(tokens)

	parser := parser.New(tokens)

	expr, err := parser.Parse()
	if err != nil {
		return err
	}

	fmt.Println(ast.Print(expr))

	value, err := interpreter.Interpret(expr)
	if err != nil {
		return err
	}

	fmt.Println(">> ", value)

	return nil
}

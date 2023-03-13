#include <lox/parser/scanner.hpp>
#include <tests/util.hpp>

#include "doctest/doctest.h"
#include <fmt/core.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using enum lox::token_type;

TEST_CASE("tokens") {
  std::string             file;
  std::vector<lox::token> want;

  SUBCASE("basic token") {
    file = "scanner/tokens.lox";
    want = {
        lox::token{PRINT, "print", 1},
        lox::token{STRING, "\"Hello, world\"", 1, "Hello, world"},
        lox::token{NUMBER, "42", 1, 42.},
        lox::token{NUMBER, "1.333", 1, 1.333},
        lox::token{TRUE, "true", 1}, // Don't store literal for booleans, can
                                     // deduce value based on token type later.
        lox::token{SEMICOLON, ";", 2},
        lox::token{EOF, "", 3},
    };
  }
  SUBCASE("identifiers") {
    file = "scanner/identifiers.lox";
    want = {
        lox::token{IDENTIFIER, "andy", 1},
        lox::token{IDENTIFIER, "formless", 1},
        lox::token{IDENTIFIER, "fo", 1},
        lox::token{IDENTIFIER, "_", 1},
        lox::token{IDENTIFIER, "_123", 1},
        lox::token{IDENTIFIER, "_abc", 1},
        lox::token{IDENTIFIER, "ab123", 1},
        lox::token{
            IDENTIFIER,
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_",
            2},
        lox::token{EOF, "", 3},
    };
  }
  SUBCASE("keywords") {
    file = "scanner/keywords.lox";
    want = {
        lox::token{AND, "and", 1},     lox::token{CLASS, "class", 1},
        lox::token{ELSE, "else", 1},   lox::token{FALSE, "false", 1},
        lox::token{FOR, "for", 1},     lox::token{FUN, "fun", 1},
        lox::token{IF, "if", 1},       lox::token{NIL, "nil", 1},
        lox::token{OR, "or", 1},       lox::token{RETURN, "return", 1},
        lox::token{SUPER, "super", 1}, lox::token{THIS, "this", 1},
        lox::token{TRUE, "true", 1},   lox::token{VAR, "var", 1},
        lox::token{WHILE, "while", 1}, lox::token{EOF, "", 2},
    };
  }
  SUBCASE("numbers") {
    file = "scanner/numbers.lox";
    want = {
        lox::token{NUMBER, "123", 1, 123.},
        lox::token{NUMBER, "123.456", 2, 123.456},
        lox::token{DOT, ".", 3},
        lox::token{NUMBER, "456", 3, 456.},
        lox::token{NUMBER, "123", 4, 123.},
        lox::token{DOT, ".", 4},
        lox::token{EOF, "", 5},
    };
  }
  SUBCASE("punctuation") {
    file = "scanner/punctuation.lox";
    want = {
        lox::token{LEFT_PAREN, "(", 1},
        lox::token{RIGHT_PAREN, ")", 1},
        lox::token{LEFT_BRACE, "{", 1},
        lox::token{RIGHT_BRACE, "}", 1},
        lox::token{SEMICOLON, ";", 1},
        lox::token{COMMA, ",", 1},
        lox::token{PLUS, "+", 1},
        lox::token{MINUS, "-", 1},
        lox::token{STAR, "*", 1},
        lox::token{BANG_EQUAL, "!=", 1},
        lox::token{EQUAL_EQUAL, "==", 1},
        lox::token{LESS_EQUAL, "<=", 1},
        lox::token{GREATER_EQUAL, ">=", 1},
        lox::token{BANG_EQUAL, "!=", 1},
        lox::token{LESS, "<", 1},
        lox::token{GREATER, ">", 1},
        lox::token{SLASH, "/", 1},
        lox::token{DOT, ".", 1},
        lox::token{EOF, "", 1},
    };
  }
  SUBCASE("whitespace") {
    file = "scanner/whitespace.lox";
    want = {
        lox::token{IDENTIFIER, "space", 1},
        lox::token{IDENTIFIER, "tabs", 1},
        lox::token{IDENTIFIER, "newlines", 1},
        lox::token{IDENTIFIER, "end", 6},
        lox::token{EOF, "", 6},
    };
  }

  const auto input = read_file(file);

  lox::scanner scanner{input};
  const auto   got = scanner.tokens();

  ::tokens_equal(want, got);
}

TEST_CASE("comments") {
  std::string             file;
  std::vector<lox::token> want;

  SUBCASE("unicode") {
    file = "scanner/unicode.lox";
    want = {
        lox::token{PRINT, "print", 9},
        lox::token{STRING, "\"ok\"", 9, "ok"},
        lox::token{SEMICOLON, ";", 9},
        lox::token{EOF, "", 9},
    };
  }
  SUBCASE("block comments") {
    file = "scanner/block.lox";
    want = {
        lox::token{NUMBER, "2.5", 1, 2.5},
        // Comments get stripped out
        lox::token{TRUE, "true", 3},
        lox::token{SEMICOLON, ";", 3},
        lox::token{EOF, "", 3},
    };
  }

  const auto input = read_file(file);

  lox::scanner scanner{input};
  const auto   got = scanner.tokens();

  ::tokens_equal(want, got);
}

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

// [Set of Keywords]
const std::unordered_set<std::string> keywords = {
	"alignas",
	"alignof",
	"auto",
	"bool",
	"break",
	"case",
	"char",
	"complex",
	"const",
	"constexpr",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"false",
	"float",
	"for",
	"goto",
	"if",
	"imaginary",
	"inline",
	"int",
	"long",
	"noreturn",
	"nullptr",
	"register",
	"restrict",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_assert",
	"struct",
	"switch",
	"thread_local",
	"true",
	"typedef",
	"typeof",
	"typeof_unqual",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	"_Alignas",
	"_Alignof",
	"_Atomic",
	"_BitInt",
	"_Bool",
	"_Complex",
	"_Decimal128",
	"_Decimal32",
	"_Decimal64",
	"_Generic",
	"_Imaginary",
	"_Noreturn",
	"_Static_assert",
	"_Thread_local",
};

// [Possible Tokens]
enum class TokenType {
    Keyword,
    Identifier,
    Integer,
	Float,
    Operator,
    String,
	Character,
    Punctuator,
    Error,
	Comment,
	EndOfFile,
};

// [Possible Errors]
enum class ErrorType {
	Null,
	Unexpected,
	EndOfFile,
	StartOfFile,
};
// [Token Format]
struct Token {
    TokenType type;     // Type of the token
    std::string value;  // Value of the token
    size_t line;        // Line number in the source code
    size_t column;      // Column number in the source code
	ErrorType error = ErrorType::Null;

    Token(TokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

// [Scanner / Lexer]
class Scanner {
public:
	const std::vector<std::string> file;
	size_t currentLine = 0;
	size_t currentColumn = -1;
    Scanner(const std::vector<std::string>& file):
        file(file) {}
    Token nextToken() {
		if (nextChar() == ErrorType::EndOfFile) {
			return Token(TokenType::EndOfFile, std::string(""), currentLine, currentColumn);
		}
		while (file[currentLine][currentColumn] == ' ' || file[currentLine][currentColumn] == '\t') {
			if (nextChar() == ErrorType::EndOfFile) {
				return Token(TokenType::EndOfFile, std::string(""), currentLine, currentColumn);
			}
		}

		// In-line Comment
		if (file[currentLine][currentColumn] == '/') {
			if (nextChar() == ErrorType::EndOfFile) {
				previousChar();
			} else if (file[currentLine][currentColumn] == '/') {
				previousChar();
				size_t commentLine = currentLine;
				size_t commentColumn = currentColumn;
				nextChar();
				while (commentLine == currentLine) {
					if (nextChar() == ErrorType::EndOfFile) {
						previousChar();
						return Token(TokenType::Comment, subString(commentLine, commentColumn, currentLine, currentColumn), commentLine, commentColumn);
					}
				}
				previousChar();
				return Token(TokenType::Comment, subString(commentLine, commentColumn, currentLine, currentColumn), commentLine, commentColumn);

		// Multi-line Comment
			} else if (file[currentLine][currentColumn] == '*') {
				previousChar();
				size_t commentLine = currentLine;
				size_t commentColumn = currentColumn;
				nextChar();
				while(file[currentLine][currentColumn] != '/') {
					while(file[currentLine][currentColumn] != '*') {
						if (nextChar() == ErrorType::EndOfFile) {
							previousChar();
							return Token(TokenType::Comment, subString(commentLine, commentColumn, currentLine, currentColumn), commentLine, commentColumn);
						}
					}
					if (nextChar() == ErrorType::EndOfFile) {
						previousChar();
						return Token(TokenType::Comment, subString(commentLine, commentColumn, currentLine, currentColumn), commentLine, commentColumn);
					}
				} 
				return Token(TokenType::Comment, subString(commentLine, commentColumn, currentLine, currentColumn), commentLine, commentColumn);
			} else { previousChar(); }
		}

		const size_t startLine = currentLine;
		const size_t startColumn = currentColumn;
		
	    // Identifier
		if (std::isalpha(file[currentLine][currentColumn]) || file[currentLine][currentColumn] == '_') {
			while(std::isalnum(file[currentLine][currentColumn]) || file[currentLine][currentColumn] == '_') {
				if (nextChar() == ErrorType::EndOfFile) { break; }
			}
			previousChar();
			return Token(TokenType::Identifier, subString(startLine, startColumn, currentLine, currentColumn), currentLine, currentColumn);
		}
	    // Integer
		if (std::isdigit(file[currentLine][currentColumn])) {
			while (std::isdigit(file[currentLine][currentColumn])) {
				if (nextChar() == ErrorType::EndOfFile) {
					return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
				}
			}
			if (file[currentLine][currentColumn] != '.') {
				previousChar();
				return Token(TokenType::Integer, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			}
		// Float
			else {
				nextChar();
				if (std::isdigit(file[currentLine][currentColumn])) {
					while (std::isdigit(file[currentLine][currentColumn])) {
						if (nextChar() == ErrorType::EndOfFile) {
							return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
						}
					}
					previousChar();
					return Token(TokenType::Float, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
				} else {
					previousChar();
					return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
				}
			}
		}
		if (file[currentLine][currentColumn] == '.') {
			nextChar();
			if (std::isdigit(file[currentLine][currentColumn])) {
				while (std::isdigit(file[currentLine][currentColumn])) {
					if (nextChar() == ErrorType::EndOfFile) {
							return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Float, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			}
			previousChar();
		}
	    // Punctuator
		switch (file[currentLine][currentColumn]){
			case '{':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '}':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '[':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case ']':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '#':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '#') {
						return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '(':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case ')':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case ';':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case ':':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == ':') {
						return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '.':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '.') {
						if (nextChar() != ErrorType::EndOfFile) {
							if (file[currentLine][currentColumn] == '.') {
								return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
							}
						}
						previousChar();
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '?':
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '-':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '>' || file[currentLine][currentColumn] == '=' || file[currentLine][currentColumn] == '-') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '~':
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '!':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '<':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
					if (file[currentLine][currentColumn] == '<') {
						if (nextChar() != ErrorType::EndOfFile) {
							if (file[currentLine][currentColumn] == '=') {
								return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
							}
						}
						previousChar();
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '>':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
					if (file[currentLine][currentColumn] == '>') {
						if (nextChar() != ErrorType::EndOfFile) {
							if (file[currentLine][currentColumn] == '=') {
								return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
							}
						}
						previousChar();
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '&':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '&') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '|':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '|') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '+':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '+') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '*':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '/':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '%':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '^':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Operator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case '=':
				if (nextChar() != ErrorType::EndOfFile) {
					if (file[currentLine][currentColumn] == '=') {
						return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
				previousChar();
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			case ',':
				return Token(TokenType::Punctuator, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
		}

		// Character
		if (file[currentLine][currentColumn] == '\'') {
			if (nextChar() != ErrorType::EndOfFile) {
				if (file[currentLine][currentColumn] == '\\') {
					if (nextChar() != ErrorType::EndOfFile) {
						if (nextChar() != ErrorType::EndOfFile) {
							if (file[currentLine][currentColumn] == '\'') {
								return Token(TokenType::Character, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
							}
						}
					}
				} else if (file[currentLine][currentColumn] != '\'') {
					if (nextChar() != ErrorType::EndOfFile) {
						if (file[currentLine][currentColumn] == '\'') {
							return Token(TokenType::Character, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
						}
					}
				}

			}
			return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);

		}

	    // String
		if (file[currentLine][currentColumn] == '"') {
			if (nextChar() == ErrorType::EndOfFile) {
				previousChar();
				return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
			}
			while(file[currentLine][currentColumn] != '"') {
				if (nextChar() == ErrorType::EndOfFile) {
					previousChar();
					return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
				}
				if (file[currentLine][currentColumn] == '\\') {
					if (nextChar() == ErrorType::EndOfFile) {
						previousChar();
						return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					} else if (nextChar() == ErrorType::EndOfFile) {
						previousChar();
						return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
					}
				}
			}
			return Token(TokenType::String, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);
		}
		return Token(TokenType::Error, subString(startLine, startColumn, currentLine, currentColumn), startLine, startColumn);

	}
	ErrorType nextChar() {
		while(true) {
			if (currentColumn + 1 < file[currentLine].size()) {
				currentColumn += 1;
				return ErrorType::Null;
			} else if (currentLine + 1 < file.size()) {
				currentLine += 1;
				currentColumn = -1;
			} else {
				return ErrorType::EndOfFile;
			}
		}
	 }
	ErrorType previousChar() {
		while(true) {
			if (currentColumn > 0) {
				currentColumn -= 1;
				return ErrorType::Null;
			} else if (currentLine > 0){
				currentLine -= 1;
				currentColumn = file[currentLine].size();
			} else {
				return ErrorType::StartOfFile;
			}
		}
	}
	std::string subString(size_t startLine, size_t startColumn, size_t endLine, size_t endColumn) {
		std::string subString = "";
		currentLine = startLine;
		currentColumn = startColumn;
		while(true)	{
			subString += file[currentLine][currentColumn];
			if (currentLine == endLine && currentColumn == endColumn) {
				return subString;
			} else if (nextChar() == ErrorType::EndOfFile) {
				std::cout << "Error: Bug, tried to index file out of bounds to create a sub string" << std::endl;
				break;
			}
		}
		return std::string("");
	}
};

// [Parser]
void parse(std::vector<std::string> file) {
	std::vector<Token> fileTokens;
	Scanner scanner(file);
	int count = 0;
	while(true) {
		count ++;
		fileTokens.push_back(scanner.nextToken());
		if ( fileTokens.back().type == TokenType::EndOfFile ) {
			break;
		} else if ( fileTokens.back().type == TokenType::Error ) {
			std::cout << "Error at Line: " << fileTokens.back().line << ", Column: " << fileTokens.back().column << std::endl;
			break;
		}
		std::cout << fileTokens.back().value << std::endl;
	}
}

// [Interface]
int main(int argc, char* argv[]) {
	using namespace std;
    string filepath;
    if (argc >= 2) {
        filepath = argv[1];
    } else {
        cout << "filepath: ";
        if (!(cin >> filepath)) {
            cerr << "No filepath provided\n";
            return 1;
        }
    }
    ifstream inputFile(filepath);

    if (!inputFile.is_open()) {
        cerr << "Error opening the file!" << endl;
        return 1;
    }

    string line = "";
	std::vector<std::string> file;
	bool firstLine = true;

    while (std::getline(inputFile, line)) {
		file.push_back(line);
    }
	parse(file);
    return 0;
}

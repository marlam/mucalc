/*
 * Copyright (C) 2015
 * Martin Lambers <marlam@marlam.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <string>

#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <muParser.h>


/* muparser custom constants */

static const double e = 2.7182818284590452353602874713526625;
static const double pi = 3.1415926535897932384626433832795029;

/* muparser custom functions */

static double deg(double x)
{
    return x * 180.0 / pi;
}

static double rad(double x)
{
    return x * pi / 180.0;
}

static double int_(double x)
{
    return (int)x;
}

static double fract(double x)
{
    return x - floor(x);
}

static double mod(double x, double y)
{
    return x - y * floor(x / y);
}

static double med(const double* x, int n)
{
    std::vector<double> values(x, x + n);
    std::sort(values.begin(), values.end());
    if (n % 2 == 1) {
        return values[n / 2];
    } else {
        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    }
}

static double clamp(double x, double minval, double maxval)
{
    return std::min(maxval, std::max(minval, x));
}

static double step(double x, double edge)
{
    return (x < edge ? 0.0 : 1.0);
}

static double smoothstep(double x, double edge0, double edge1)
{
    double t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - t * 2.0);
}

static double mix(double x, double y, double t)
{
    return x * (1.0 - t) + y * t;
}

static double unary_plus(double x)
{
    return x;
}

/* muparser implicit variable definitions */

static std::vector<std::pair<std::string, std::unique_ptr<double>>> added_vars;

static double* add_var(const char* name, void* /* data */)
{
    added_vars.push_back(std::make_pair(
                std::string(name), std::unique_ptr<double>(new double(0.0))));
    return added_vars.back().second.get();
}

/* muparser evaluation of an expression and printing of result */

static int eval_and_print(mu::Parser& parser, const char* expr)
{
    int retval = 0;
    try {
        parser.SetExpr(expr);
        int n;
        double* results = parser.Eval(n);
        for (int j = 0; j < n; j++) {
            printf("%.17g%s", results[j], j == n - 1 ? "\n" : ", ");
        }
    }
    catch (mu::Parser::exception_type& e) {
        fprintf(stderr, "%s\n", e.GetMsg().c_str());
        retval = 1;
    }
    return retval;
}

/* readline custom completion */

static const char* constant_names[] = {
    "pi", "e",
    NULL
};

static const char* function_names[] = {
    "deg", "rad",
    "sin", "asin", "cos", "acos", "tan", "atan", "atan2",
    "sinh", "asinh", "cosh", "acosh", "tanh", "atanh",
    "pow", "exp", "exp2", "exp10", "log", "ln", "log2", "log10",
    "sqrt", "cbrt", "abs", "sign",
    "fract", "int", "ceil", "floor", "round", "rint", "trunc", "mod",
    "min", "max", "sum", "avg", "med",
    "clamp", "step", "smoothstep", "mix",
    NULL
};

static char* completion_generator(const char* text, int state)
{
    static int functions_index, constants_index, variables_index, len;
    if (state == 0) {
        functions_index = 0;
        constants_index = 0;
        variables_index = 0;
        len = strlen(text);
    }

    const char* name;
    // first complete function names...
    while ((name = function_names[functions_index])) {
        functions_index++;
        if (strncmp(name, text, len) == 0)
            break;
    }
    if (name) {
        rl_completion_append_character = '(';
        return strdup(name);
    }
    // ... then constant names...
    while ((name = constant_names[constants_index])) {
        constants_index++;
        if (strncmp(name, text, len) == 0)
            break;
    }
    if (name) {
        rl_completion_append_character = ' ';
        return strdup(name);
    }
    // ... and finally variable names.
    while (static_cast<size_t>(variables_index) < added_vars.size()) {
        name = added_vars[variables_index].first.c_str();
        variables_index++;
        if (strncmp(name, text, len) == 0)
            break;
        name = NULL;
    }
    if (name) {
        rl_completion_append_character = ' ';
        return strdup(name);
    }
    return NULL;
}

/* readline history file location */

std::string history_file()
{
    static std::string histfile;
    if (histfile.empty()) {
#ifdef _WIN32
        const char* appdata = getenv("APPDATA");
        if (appdata)
            histfile = std::string(appdata) + '\\';
        histfile += "mucalc_history.txt";
#else
        const char* home = getenv("HOME");
        if (home)
            histfile = std::string(home) + '/';
        histfile += ".mucalc_history";
#endif
    }
    return histfile;
}

/* main() */

void print_short_version()
{
    printf("mucalc version 1.1\n");
}

void print_short_help()
{
    printf("Type an expression, 'help', or 'quit'.\n");
}

void print_core_help()
{
    printf("Evaluates mathematical expression(s) and prints the results.\n");
    printf("Expressions can be given as arguments, read from an input stream, or\n");
    printf("typed interactively.\n");
    printf("The evaluation is handled by muparser <http://muparser.beltoforion.de/>.\n");
    printf("Variables can be used without explicit declaration. Separating multiple\n");
    printf("expressions with commas is supported.\n");
    printf("Available constants:\n");
    printf("  pi, e\n");
    printf("Available functions:\n");
    printf("  deg, rad,\n");
    printf("  sin, asin, cos, acos, tan, atan, atan2,\n");
    printf("  sinh, asinh, cosh, acosh, tanh, atanh,\n");
    printf("  pow, exp, exp2, exp10, log, ln, log2, log10, sqrt, cbrt,\n");
    printf("  abs, sign, fract, int, ceil, floor, round, rint, trunc, mod,\n");
    printf("  min, max, sum, avg, med,\n");
    printf("  clamp, step, smoothstep, mix\n");
    printf("Available operators:\n");
    printf("  ^, *, /, +, -, ==, !=, <, >, <=, >=, ||, &&, ?:\n");
    printf("Expression examples:\n");
    printf("  sin(pi/2)\n");
    printf("  sin(rad(90))\n");
    printf("  a = 2^3 + 2\n");
    printf("  b = sqrt(49) * 2 + 6\n");
    printf("  sin(2 * pi) + a * b / log10(a^(b/4)) + cos(rad(12*(a+b))) + sign(a)\n");
}

int main(int argc, char *argv[])
{
    int retval = 0;

    // --version, --help
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        print_short_version();
        printf("Copyright (C) 2015 Martin Lambers <marlam@marlam.de>\n");
        printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
        printf("This is free software: you are free to change and redistribute it.\n");
        printf("There is NO WARRANTY, to the extent permitted by law.\n");
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [<expression...>]\n", argv[0]);
        printf("\n");
        print_core_help();
        printf("\n");
        printf("Report bugs to <marlam@marlam.de>.\n");
        return 0;
    }

    // Initialize the parser
    mu::Parser parser;
    parser.ClearConst();
    parser.DefineConst("e", e);
    parser.DefineConst("pi", pi);
    parser.DefineFun("deg", deg);
    parser.DefineFun("rad", rad);
    parser.DefineFun("atan2", atan2);
    parser.DefineFun("fract", fract);
    parser.DefineFun("pow", pow);
    parser.DefineFun("exp2", exp2);
    parser.DefineFun("cbrt", cbrt);
    parser.DefineFun("int", int_);
    parser.DefineFun("ceil", ceil);
    parser.DefineFun("floor", floor);
    parser.DefineFun("round", round);
    parser.DefineFun("trunc", trunc);
    parser.DefineFun("mod", mod);
    parser.DefineFun("med", med);
    parser.DefineFun("clamp", clamp);
    parser.DefineFun("step", step);
    parser.DefineFun("smoothstep", smoothstep);
    parser.DefineFun("mix", mix);
    parser.DefineInfixOprt("+", unary_plus);
    parser.SetVarFactory(add_var, NULL);

    // Evaluate command line expression(s)
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            retval = eval_and_print(parser, argv[i]);
        }
        return retval;
    }

    // Evaluate standard input
    if (isatty(fileno(stdin))) {
        // interactive: use readline()
        rl_sort_completion_matches = 0;
        rl_basic_word_break_characters = " ()+-*/^?:,=!<>|&\t";
        rl_basic_quote_characters = "";
        rl_completion_entry_function = completion_generator;
        stifle_history(1000);
        read_history(history_file().c_str());
        char* line;
        bool quit_via_control_d = true;
        print_short_version();
        print_short_help();
        while ((line = readline("> "))) {
            std::string string_line = line;
            std::string trimmed_line;
            size_t first_nonspace = string_line.find_first_not_of(' ');
            if (first_nonspace != std::string::npos) {
                size_t last_nonspace = string_line.find_last_not_of(' ');
                trimmed_line = string_line.substr(first_nonspace, last_nonspace - first_nonspace + 1);
            }
            if (!trimmed_line.empty())
                add_history(line);
            if (trimmed_line.empty()) {
                print_short_help();
            } else if (trimmed_line == "help" || trimmed_line == "?") {
                print_core_help();
            } else if (trimmed_line == "quit" || trimmed_line == "exit") {
                quit_via_control_d = false;
                break;
            } else {
                retval = eval_and_print(parser, line);
            }
            free(line);
        }
        if (quit_via_control_d)
            printf("^D\n");
        free(line);
        write_history(history_file().c_str());
    } else {
        // use std::getline()
        do {
            std::string line;
            std::getline(std::cin, line);
            if (std::cin)
                retval = eval_and_print(parser, line.c_str());
        }
        while (std::cin);
    }
    return retval;
}

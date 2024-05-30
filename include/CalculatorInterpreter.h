#pragma once
#include <iostream>
#include "TParserBaseListener.h"
#include "TParser.h"
#include "TLexer.h"
#include "Polynomial.h"
#include <stack> 
#include <map>

using namespace std;

namespace antlrcpptest {
    class CalculatorInterpreter : public TParserBaseListener {
        stack<Polynomial> polynomial_stack;
        map<string, Polynomial> parameters;
        stack<string> var_stack;
        stack<int> integ;

        int X_degree;
        int Y_degree;
        int Z_degree;

        stack<string> mon_var;

    public:
        CalculatorInterpreter() : X_degree(0), Y_degree(0), Z_degree(0) {}

        void exitPlusOp(TParser::PlusOpContext* ctx) override {
            if (polynomial_stack.size() < 2) return;
            Polynomial r = move(polynomial_stack.top()); polynomial_stack.pop();
            Polynomial l = move(polynomial_stack.top()); polynomial_stack.pop();
            polynomial_stack.push(move(l + r));
        }

        void exitBinaryMinusOp(TParser::BinaryMinusOpContext* ctx) override {
            if (polynomial_stack.size() < 2) return;
            Polynomial r = move(polynomial_stack.top()); polynomial_stack.pop();
            Polynomial l = move(polynomial_stack.top()); polynomial_stack.pop();
            polynomial_stack.push(move(l - r));
        }

        void exitUnaryMinusOp(TParser::UnaryMinusOpContext* ctx) override {
            if (polynomial_stack.empty()) return;
            polynomial_stack.top() = move(polynomial_stack.top() * (-1));
        }

        void exitMultOp(TParser::MultOpContext* ctx) override {
            if (polynomial_stack.size() < 2) return;
            Polynomial r = move(polynomial_stack.top()); polynomial_stack.pop();
            Polynomial l = move(polynomial_stack.top()); polynomial_stack.pop();
            polynomial_stack.push(move(l * r));
        }

        void exitSetValue(TParser::SetValueContext* ctx) override {
            if (polynomial_stack.empty() || var_stack.empty()) return;
            Polynomial cur = move(polynomial_stack.top());
            string var_name = move(var_stack.top());
            parameters[var_name] = move(cur);
            polynomial_stack.pop();
            var_stack.pop();
        }

        void exitGetValue(TParser::GetValueContext* ctx) override {
            if (var_stack.empty()) return;
            string var_name = move(var_stack.top());
            var_stack.pop();
            try {
                Polynomial cur = parameters.at(var_name);
                polynomial_stack.push(move(cur));
            }
            catch (const out_of_range&) {
                string message = var_name + " not found";
                throw out_of_range(message);
            }
        }

        void exitConvertToInt(TParser::ConvertToIntContext* ctx) override {
            integ.push(stoi(ctx->getText()));
        }

        void exitReadUserVar(TParser::ReadUserVarContext* ctx) override {
            var_stack.push(ctx->getText());
        }

        void exitPrint(TParser::PrintContext* ctx) override {
            if (polynomial_stack.empty()) return;
            polynomial_stack.top().print();
            cout << endl;
            polynomial_stack.pop();
        }

        void exitReadVar(TParser::ReadVarContext* ctx) override {
            mon_var.push(ctx->getText());
        }

        void exitPow(TParser::PowContext* ctx) override {
            if (integ.empty() || mon_var.empty()) return;
            int pow = integ.top();
            string var = mon_var.top();
            if (var == "x") {
                X_degree += pow;
            }
            else if (var == "y") {
                Y_degree += pow;
            }
            else if (var == "z") {
                Z_degree += pow;
            }
            integ.pop();
            mon_var.pop();
        }

        void enterMonom(TParser::MonomContext* ctx) override {
            X_degree = Y_degree = Z_degree = 0;
        }

        void exitFullMonom(TParser::FullMonomContext* ctx) override {
            if (integ.empty()) return;
            int coeff = integ.top(); integ.pop();
            Polynomial polynom;
            polynom.addMonom(coeff, 100 * X_degree + 10 * Y_degree + Z_degree);
            polynomial_stack.push(move(polynom));
        }

        void exitMonomZeroPower(TParser::MonomZeroPowerContext* ctx) override {
            if (integ.empty()) return;
            int coeff = integ.top(); integ.pop();
            Polynomial polynom;
            polynom.addMonom(coeff, 0);
            polynomial_stack.push(move(polynom));
        }

        void enterMonomWithouCoefficient(TParser::MonomWithouCoefficientContext* ctx) override {
            Polynomial polynom;
            polynom.addMonom(1, 100 * X_degree + 10 * Y_degree + Z_degree);
            polynomial_stack.push(move(polynom));
        }

        void exitCalculate(TParser::CalculateContext* ctx) override {
            if (integ.size() < 3 || polynomial_stack.empty()) return;
            int x_point = integ.top(); integ.pop();
            int y_point = integ.top(); integ.pop();
            int z_point = integ.top(); integ.pop();
            Polynomial cur = move(polynomial_stack.top()); polynomial_stack.pop();
            int result = cur.calculate(x_point, y_point, z_point);
            cout << "Calc in (" << x_point << ", " << y_point << ", " << z_point << ") = " << result << endl;
        }
    };
}

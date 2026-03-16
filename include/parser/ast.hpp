#pragma once
#include <string>
#include <variant>
#include <vector>
#include "binary_op_node.hpp"
#include "node.hpp"
#include "postfix_op_node.hpp"
#include "type_node.hpp"
#include "unary_op_node.hpp"
struct AST {
    Node node;
    std::variant<
        int,
        double,
        bool,
        std::string,
        BinaryOpNode,
        PostfixOpNode,
        TypeNode,
        UnaryOpNode
    > v;
    std::vector<std::unique_ptr<AST>> children;
};

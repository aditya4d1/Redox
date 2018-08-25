//
// Copyright 2018 - present @adityaatluri
//

#pragma once

class NExpression;
class NStatement;
class NInteger;
class NDouble;
class NIdentifier;
class NMethodCall;
class NVariableDeclaration;
class NBinaryOperator;
class NAssignment;
class NBlock;
class NFunctionDeclaration;
class NExpressionStatement;

class Visitor {
public:
virtual void visitExpression(NExpression* n) = 0;
virtual void visitStatement(NStatement* n) = 0;
virtual void visitInteger(NInteger* n) = 0;
virtual void visitDouble(NDouble* n) = 0;
virtual void visitIdentifier(NIdentifier* n) = 0;
virtual void visitMethodCall(NMethodCall* n) = 0;
virtual void visitBinaryOperator(NBinaryOperator* n) = 0;
virtual void visitAssignment(NAssignment* n) = 0;
virtual void visitBlock(NBlock* n) = 0;
virtual void visitExpressionStatement(NExpressionStatement* n) = 0;
virtual void visitVariableDeclaration(NVariableDeclaration* n) = 0;
virtual void visitFunctionDeclaration(NFunctionDeclaration* n) = 0;
};
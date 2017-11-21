/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Optimiser component that makes all identifiers unique.
 */

#pragma once

#include <libjulia/Aliases.h>

#include <libsolidity/inlineasm/AsmAnalysisInfo.h>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <set>

namespace dev
{
namespace julia
{
class EVMAssembly;

/**
 * Creates a copy of a iulia AST replacing all identifiers by unique names.
 */
class Disambiguator: public boost::static_visitor<Statement>
{
public:
	Disambiguator(
		solidity::assembly::Block const& _block,
		solidity::assembly::AsmAnalysisInfo const& _analysisInfo
	):
		m_block(_block), m_info(_analysisInfo)
	{}

	std::shared_ptr<solidity::assembly::Block> run();

public:
	Statement operator()(Literal const& _literal);
	Statement operator()(Instruction const& _instruction);
	Statement operator()(Identifier const& _identifier);
	Statement operator()(FunctionalInstruction const& _instr);
	Statement operator()(FunctionCall const&);
	Statement operator()(Label const& _label);
	Statement operator()(StackAssignment const& _assignment);
	Statement operator()(Assignment const& _assignment);
	Statement operator()(VariableDeclaration const& _varDecl);
	Statement operator()(If const& _if);
	Statement operator()(Switch const& _switch);
	Statement operator()(FunctionDefinition const&);
	Statement operator()(ForLoop const&);
	Statement operator()(Block const& _block);

private:
	class TemporaryScope;

	template <typename T>
	std::vector<T> translateVector(std::vector<T> const& _values);

	template <typename T>
	std::shared_ptr<T> translate(std::shared_ptr<T> const& _v) { return _v ? std::make_shared<T>(translate(*_v)) : nullptr; }
	Statement translate(Statement const& _statement);
	Block translate(Block const& _block);
	Case translate(Case const& _case);
	Identifier translate(Identifier const& _identifier);
	Literal translate(Literal const& _literal);
	TypedName translate(TypedName const& _typedName);
	std::string translateIdentifier(std::string const& _name);

	Block const& m_block;
	solidity::assembly::AsmAnalysisInfo const& m_info;

	solidity::assembly::Scope* m_scope = nullptr;
	std::map<void const*, std::string> m_translations;
	std::set<std::string> m_usedNames;
};

template <typename T>
std::vector<T> Disambiguator::translateVector(std::vector<T> const& _values)
{
	std::vector<T> translated;
	for (auto const& v: _values)
		translated.emplace_back(translate(v));
	return translated;
}


}
}

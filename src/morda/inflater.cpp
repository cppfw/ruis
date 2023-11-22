/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "inflater.hpp"

#include "util/util.hpp"

#include "context.hpp"

using namespace std::string_literals;

using namespace morda;

inflater::inflater(morda::context& context) :
	context(context)
{}

void inflater::add_factory(std::string widget_name, decltype(factories)::value_type::second_type factory)
{
	auto ret = this->factories.insert(std::make_pair(std::move(widget_name), std::move(factory)));
	if (!ret.second) {
		throw std::logic_error("Failed registering widget type, widget type with given name is already added");
	}
}

bool inflater::unregister_widget(const std::string& widget_name) noexcept
{
	if (this->factories.erase(widget_name) == 0) {
		return false;
	}
	return true;
}

utki::shared_ref<morda::widget> inflater::inflate(const papki::file& fi)
{
	return this->inflate(treeml::read(fi));
}

namespace {
constexpr const char* wording_defs = "defs";
} // namespace

namespace {
void substitute_vars(
	treeml::forest& to,
	const std::function<const treeml::forest*(const std::string&)>& find_var,
	bool blank_not_found_vars,
	bool go_beyond_child_widgets
)
{
	ASSERT(find_var)

	for (auto i = to.begin(); i != to.end();) {
		// TRACE(<< "i->value = " << i->value.to_string() << std::endl)
		if (i->value == "$") {
			if (i->children.empty()) {
				throw std::invalid_argument(
					"malformed GUI definition: error: reference to a variable holds no variable name"
				);
			}

			if (i->children.size() != 1) {
				throw std::invalid_argument(
					"malformed GUI definition: reference to variable holds more than one variable name"
				);
			}

			if (!i->children.front().children.empty()) {
				throw std::invalid_argument("malformed GUI definition: variable name has children");
			}

			if (auto var = find_var(i->children.front().value.to_string())) {
				i = to.insert(i, var->begin(), var->end());
				i = std::next(i, std::distance(var->begin(), var->end()));
				i = to.erase(i);
				continue;
			}

			if (blank_not_found_vars) {
				i = to.erase(i);
				continue;
			}
		} else {
			if (i->value.empty() || go_beyond_child_widgets || i->value[0] != '@') {
				substitute_vars(i->children, find_var, blank_not_found_vars, true);
			}
		}
		++i;
	}
}
} // namespace

namespace {
treeml::forest apply_gui_template(
	const treeml::forest& templ,
	const std::set<std::string>& var_names,
	treeml::forest trees
)
{
	std::cout << "applying template: " << treeml::to_string(templ) << std::endl;
	std::cout << "trees: " << tml::to_string(trees) << std::endl;
	treeml::forest ret = templ;

	std::map<std::string, treeml::forest> vars;
	treeml::forest children;
	for (auto& i : trees) {
		if (!is_leaf_property(i.value)) {
			children.emplace_back(std::move(i));
			continue;
		}

		if (var_names.find(i.value.to_string()) != var_names.end()) {
			vars[i.value.to_string()] = std::move(i.children);
			continue;
		}

		auto t = std::find(ret.begin(), ret.end(), i.value);
		if (t != ret.end()) {
			t->children = std::move(i.children);
			continue;
		}

		ret.emplace_back(std::move(i));
	}
	vars["children"] = std::move(children);

	#ifdef DEBUG
		for(auto& v : vars){
			std::cout << "v = " << v.first << std::endl;
		}
		std::cout << "ret = " << treeml::to_string(ret) << std::endl;
	#endif

	substitute_vars(
		ret,
		[&vars](const std::string& name) -> const treeml::forest* {
			// TRACE(<< "looking for var = " << name << std::endl)
			auto i = vars.find(name);
			if (i == vars.end()) {
				// TRACE(<< "not found" << std::endl)
				return nullptr;
			}
			// TRACE(<< "found = " << treeml::to_string(i->second) << std::endl)
			return &i->second;
		},
		false,
		true
	);

	std::cout << "ret substed = " << treeml::to_string(ret) << std::endl;

	return ret;
}
} // namespace

const decltype(inflater::factories)::value_type::second_type& inflater::find_factory(const std::string& widget_name)
{
	auto i = this->factories.find(widget_name);

	if (i == this->factories.end()) {
		std::stringstream ss;
		ss << "inflater::find_factory(): widget name '" << widget_name << "' not found";
		throw std::logic_error(ss.str());
	}

	return i->second;
}

utki::shared_ref<widget> inflater::inflate(const char* str)
{
	return this->inflate(treeml::read(str));
}

utki::shared_ref<widget> inflater::inflate(const std::string& str)
{
	return this->inflate(str.c_str());
}

utki::shared_ref<widget> inflater::inflate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end)
{
	auto i = begin;

	if (i == end) {
		throw std::invalid_argument("inflater::inflater(): widget declaration not found in supplied forest");
	}

	if (is_leaf_property(i->value) || i->value.empty() || i->value.to_string()[0] != '@') {
		throw std::invalid_argument(
			"inflater::inflater(): widget declaration must go first, found: "s + i->value.to_string()
		);
	}

	std::string widget_name;
	treeml::forest widget_desc;

	// TRACE(<< "inflating = " << i->value.to_string() << std::endl)
	if (auto tmpl = this->find_template(i->value.to_string().substr(1))) {
		widget_name = tmpl->templ.value.to_string().substr(1);
		std::cout << "i->children = " << tml::to_string(i->children) << std::endl;
		widget_desc = apply_gui_template(
			tmpl->templ.children,
			tmpl->vars,
			treeml::forest(i->children) // copy children
		);
		// TRACE(<< "After applying template: " << treeml::to_string(widget_desc) << std::endl)
	} else {
		widget_name = i->value.to_string().substr(1);
		widget_desc = i->children;
	}

	auto fac = this->find_factory(widget_name);

	unsigned num_pop_defs = 0;
	utki::scope_exit pop_defs_scope_exit([this, &num_pop_defs]() {
		for (unsigned i = 0; i != num_pop_defs; ++i) {
			this->pop_defs_block();
		}
	});

	for (auto& d : widget_desc) {
		if (d.value != wording_defs) {
			continue;
		}
		// TRACE(<< "push local defs: " << treeml::to_string(d.children) << std::endl)
		this->push_defs_block(d.children);
		++num_pop_defs;

		// OPTIMIZATION: clear defs block to preven unnecessary variables substitution inside the defs block later
		d.children.clear();
	}

	substitute_vars(
		widget_desc,
		[this](const std::string& name) -> const treeml::forest* {
			return this->find_variable(name);
		},
		true,
		false
	);

	try {
		return fac(utki::make_shared_from(this->context), widget_desc);
	} catch (...) {
		LOG([&](auto& o) {
			o << "could not inflate widget: " << widget_name << "{" << treeml::to_string(widget_desc) << "}"
			  << std::endl;
		})
		throw;
	}
}

namespace {
// name starts with @
void check_template_recursion(const std::string& name, const treeml::forest& desc)
{
	for (auto& c : desc) {
		if (is_leaf_child(c.value)) {
			if (c.value == name) {
				std::stringstream ss;
				ss << "error: template '" << name << "' has recursion";
				throw std::logic_error(ss.str());
			}
			check_template_recursion(name, c.children);
		}
	}
}
} // namespace

inflater::widget_template inflater::parse_template(const std::string& name, const treeml::forest& templ)
{
	// TRACE(<< "parse_template(): templ = " << treeml::to_string(templ) << std::endl)
	widget_template ret;

	for (auto& n : templ) {
		// template definition
		if (!ret.templ.value.empty()) {
			break;
		}

		if (is_leaf_property(n.value)) {
			// possibly variable name
			if (!n.children.empty()) {
				throw std::invalid_argument("inflater::parse_template(): template argument name has children");
			}
			ret.vars.insert(n.value.to_string());
			continue;
		}

		ASSERT(!is_leaf_property(n.value))
		// TRACE(<< "template name = " << n.value.to_string() << std::endl)

		ret.templ = n;
	}
	if (ret.templ.value.empty()) {
		throw std::invalid_argument("inflater::parse_template(): template has no definition");
	}
	// TRACE(<< "template definition found: " << treeml::to_string(ret.templ) << std::endl)

	// TRACE(<< "vars:" << std::endl)
	// for each variable create a stub property if needed
	for (auto& v : ret.vars) {
		// TRACE(<< " " << v << std::endl)
		auto i = std::find(ret.templ.children.begin(), ret.templ.children.end(), v);
		if (i == ret.templ.children.end()) {
			ret.templ.children.emplace_back(
				// tml::leaf(v), {tml::tree()} // TODO: is empty child needed?
				tml::leaf(v)
			);
		}
	}

	ASSERT(!ret.templ.value.empty())
	ASSERT(ret.templ.value.to_string()[0] == '@')

	check_template_recursion(name, ret.templ.children);

	if (auto tmpl = this->find_template(ret.templ.value.to_string().substr(1))) {
		ret.templ.value = tmpl->templ.value;
		ret.templ.children = apply_gui_template(tmpl->templ.children, tmpl->vars, std::move(ret.templ.children));

		ret.vars.insert(tmpl->vars.begin(), tmpl->vars.end()); // forward all variables
	}

	// TRACE(<< "template parsed: " << treeml::to_string(ret.templ) << std::endl)
	return ret;
}

void inflater::push_defs(treeml::forest::const_iterator begin, treeml::forest::const_iterator end)
{
	for (auto i = begin; i != end; ++i) {
		// TRACE(<< "inflater::inflate(): i->value = " << i->value.to_string() << std::endl)
		if (i->value == wording_defs) {
			this->push_defs_block(i->children);
		} else {
			throw std::invalid_argument(
				"inflater::push_defs(): unknown declaration encountered: "s + i->value.to_string()
			);
		}
	}
}

void inflater::push_defs(const treeml::forest& chain)
{
	this->push_defs(chain.begin(), chain.end());
}

void inflater::push_defs(const char* str)
{
	this->push_defs(treeml::read(str));
}

void inflater::push_defs_block(const treeml::forest& chain)
{
	this->push_variables(chain);
	this->push_templates(chain);
}

void inflater::pop_defs_block()
{
	this->pop_variables();
	this->pop_templates();
}

void inflater::push_templates(const treeml::forest& chain)
{
	decltype(this->templates)::value_type m;

	for (auto& c : chain) {
		if (is_property(c)) {
			// skip variables
			continue;
		}

		if (c.children.empty()) {
			throw std::invalid_argument("inflater::push_templates(): template name has no children, error.");
		}
		// TRACE(<< "pushing template: " << treeml::to_string(c.children) << std::endl)

		ASSERT(!c.value.empty())
		ASSERT(c.value.to_string()[0] == '@')

		const auto& name = c.value.to_string();

		auto res = m.insert(std::make_pair(name.substr(1), parse_template(name, c.children)));

		if (!res.second) {
			throw std::invalid_argument(
				"inflater::push_templates(): template name is already defined in given templates chain, error."
			);
		}
	}

	this->templates.push_back(std::move(m));
}

void inflater::pop_templates()
{
	ASSERT(this->templates.size() != 0)
	this->templates.pop_back();
}

const inflater::widget_template* inflater::find_template(const std::string& name) const
{
	for (auto i = this->templates.rbegin(); i != this->templates.rend(); ++i) {
		auto r = i->find(name);
		if (r != i->end()) {
			return &r->second;
		}
	}
	//	TRACE(<< "inflater::find_template(): template '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}

const treeml::forest* inflater::find_variable(const std::string& name) const
{
	for (auto i = this->variables.rbegin(); i != this->variables.rend(); ++i) {
		auto r = i->find(name);
		if (r != i->end()) {
			return &r->second;
		}
	}
	//	TRACE(<< "inflater::find_variable(): variable '" << name <<"' not found!!!" << std::endl)
	return nullptr;
}

void inflater::pop_variables()
{
	ASSERT(this->variables.size() != 0)
	this->variables.pop_back();
}

void inflater::push_variables(const treeml::forest& defs)
{
	decltype(this->variables)::value_type m;

	for (auto& t : defs) {
		if (!is_property(t)) {
			continue;
		}

		auto value = t.children;

		substitute_vars(
			value,
			[this](const std::string& name) -> const treeml::forest* {
				return this->find_variable(name);
			},
			true,
			true
		);

		if (!m.insert(std::make_pair(t.value.to_string(), std::move(value))).second) {
			std::stringstream ss;
			ss << "inflater::push_variables(): failed to add variable '" << t.value.to_string()
			   << "', variable with same name is already defined in this defs block";
			throw std::logic_error(ss.str());
		}
	}

	this->variables.push_back(std::move(m));
}

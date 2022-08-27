#include "tml.h"
#include "QDir"
#include "QTextStream"

TmlNode::~TmlNode() {
	clear();
}

void TmlNode::clear() 
{
	for (int i = 0; i < childs.size(); i++) 
	{
		childs[i]->clear();
		delete childs[i];
	}

	childs.clear();

	for (int i = 0; i < args.size(); i++) {
		delete args[i];
	}
	args.clear();

	empty_element = false;
}

TmlArgument* TmlNode::getArg(const QString& _name) const {
	for (int i = 0; i < args.size(); i++) {
		if (args[i]->getName() == _name)
			return args[i];
	}
	return NULL;
}

void TmlNode::setArgValue(const QString& name, const QString& value)
{
	TmlArgument* arg = getArg(name);

	if (arg) {
		arg->setValue(value);
	}
	else {
		arg = new TmlArgument();
		arg->setName(name);
		arg->setValue(value);
		args.append(arg);
	}
}

QString TmlNode::getArgValue(const QString& name) {
	TmlArgument* arg = getArg(name);

	if (arg) {
		return arg->getValue();
	}

	return "";
}

TmlNode* TmlNode::getChild(const QString& name) {
	for (int i = 0; i < childs.size(); i++) {
		TmlNode* child = childs[i];
		if (child->name == name)
			return child;
	}
	return NULL;
}

int TmlNode::getChildIndex(const QString& name) {
	for (int i = 0; i < childs.size(); i++) {
		if (childs[i]->name == name)
			return i;
	}
	return -1;
}

TmlNode* TmlNode::createChild(const QString& _name) {
	TmlNode* result = new TmlNode();
	result->setName(_name);
	result->parent = this;
	childs.append(result);
	return result;
}

bool TmlNode::removeChild(int index) {
	if (index < 0)
		return false;

	if (index >= childs.size())
		return false;

	delete childs[index];
	childs.remove(index);
	return true;
}

bool TmlNode::parse(const QString& src) 
{
	TmlStringSource* source = new TmlStringSource(src);
	parse(source);
	bool result = !source->error;
	delete source;

	return result;;
}

bool TmlNode::loadFile(const QString& path) {
	QFile file(path);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	QString text;

	QTextStream in(&file);
	in.setCodec("UTF-8");

	//text = in.readAll();

	while (!in.atEnd())
	{
		QString line = in.readLine();
		text += line;
		if (!in.atEnd()) {
			text += "\r\n";
		}
	}

	in.flush();
	file.close();

	return parse(text);
}

void TmlNode::skipWhitespace(TmlSource* source) {
	
	while (source->ch == ' ' || source->ch == '\t' || source->ch == TmlSource::EOL) {
		source->next();
	}
}

QString TmlNode::parseIdent(TmlSource* source) {
	QString node_name;

	if ((source->ch >= 'a' && source->ch <= 'z') || (source->ch >= 'A' && source->ch <= 'Z') || source->ch == '_') {
		node_name = source->ch;

		source->next();
		while ((source->ch >= 'a' && source->ch <= 'z') || (source->ch >= 'A' && source->ch <= 'Z') || (source->ch >= '0' && source->ch <= '9') || source->ch == '_') {
			node_name += source->ch;
			source->next();
		}

	}
	return node_name;
}

QString TmlNode::parseArgValue(TmlSource* source) {
	QString arg_value;

	while (source->ch != '\"' && source->ch != '<' && source->ch != '>') {
		arg_value += source->ch;
		source->next();
	}

	return arg_value;
}

QString TmlNode::parseData(TmlSource* source) {
	QString arg_value;

	while (source->ch != '\"' && source->ch != '<' && source->ch != '>') {
		arg_value += source->ch;
		source->next();
	}

	return arg_value;
}

void TmlNode::parseNode(TmlSource* source, TmlNode* parent) {
	
	skipWhitespace(source);

	if (source->ch == '<') {
		TmlNode* node;
			
		if (parent == NULL)
			node = this;
		else
			node = parent->createChild("");
		
		node->line = source->line;
		node->pos = source->pos;

		source->next();
		QString node_name = parseIdent(source);

		if  (node_name == "") {
			source->doError("Expected node name");
			return;
		}

		node->setName(node_name);
		skipWhitespace(source);

		int arg_line = source->line;
		int arg_pos = source->pos;

		QString arg_name = parseIdent(source);

		while (arg_name != "") {

			if (node->getArg(arg_name) != NULL) {
				source->doError("Arg olredy exist", arg_line, arg_pos);
				return;
			}

			if (source->ch != '=') {
				source->doError("Expected '='");
				return;
			}

			source->next();

			QString arg_value;

			if (source->ch != '"') {
				source->doError("Expected '\"'");
				return;
			}
			source->next();

			arg_value = parseArgValue(source);

			if (source->ch != '\"') {
				source->doError("Expected '\"'");
				return;
			}

			TmlArgument* arg = new TmlArgument();
			arg->pos = arg_pos;
			arg->line = arg_line;
			arg->setName(arg_name);
			QString decodedArgValue = decodeString(arg_value);
			arg->setValue(decodedArgValue);
			node->args.append(arg);

			source->next();
			skipWhitespace(source);

			arg_line = source->line;
			arg_pos = source->pos;

			arg_name = parseIdent(source);
		}

		if (source->ch == '/') {
			node->setEmptyElement(true);
			source->next();
		}

		if (source->ch != '>') {
			source->doError("Expected '>'");
			return;
		}

		source->next();

		if (!node->getEmptyElement()) {
			skipWhitespace(source);

			if (source->ch == '<') {
				while (source->ch == '<') {

					if (source->forward() == '/') {
						source->next();
						source->next();

						QString node_name2 = parseIdent(source);
						if (node_name != node_name2) {
							source->doError("Expected ident");
							return;
						}

						if (source->ch != '>') {
							source->doError("Expected '>'");
							return;
						}
						source->next();
						return;
					}
					else {
						parseNode(source, node);

					}
					skipWhitespace(source);
				}
			}
			else {
				QString data = parseData(source);
				QString decodedData = decodeString(data);
				node->setData(decodedData);

				if (source->ch == '<') {
					source->next();

					if (source->ch == '/') {
						source->next();

						QString node_name2 = parseIdent(source);
						if (node_name != node_name2) {
							source->doError("Expected ident");
							return;
						}

						if (source->ch != '>') {
							source->doError("Expected '>'");
							return;
						}
						source->next();
						return;
					}
					else {
						source->doError("Expected '/'");
						return;
					}
				}
				else {
					source->doError("Expected '<'");
					return;
				}
			}

		}
		else {
			return;
		}

	}
	else {
		source->doError("Expected '<'");
		return;
	}
}

void TmlNode::parse(TmlSource* source) 
{
	if (source == NULL)
		return;

	clear();
	parseNode(source, NULL);
}

int TmlNode::nodeDephth(TmlNode* node, TmlNode* root) {
	
	int result = 0;

	while (node != NULL) {
		if (node == root)
			break;
		result++;
		node = node->parent;
	}

	return result;
}

QString TmlNode::genSpaces(int num) {
	QString result;

	for (int i = 0; i < num; i++)
		result += " ";

	return result;
}

QString TmlNode::decodeString(const QString& in) {
	QString out;
	int i = 0;

	while (i < in.size()) {
		if (in[i] == '&') {
			i++;
			if (in.mid(i, 3) == "lt;") { i += 3; out += '<'; }
			else if (in.mid(i, 3) == "gt;") { i += 3; out += '>'; }
			else if (in.mid(i, 4) == "amp;") { i += 4; out += '&'; }
			else if (in.mid(i, 5) == "nbsp;") { i += 5; out += ' '; }
			else if (in.mid(i, 5) == "quot;") { i += 5; out += '"'; }
			else if (in.mid(i, 5) == "apos;") { i += 5; out += '\''; }
			else if (in.mid(i, 4) == "#34;") { i += 4; out += '"'; }
			else if (in.mid(i, 4) == "#38;") { i += 4; out += '&'; }
			else if (in.mid(i, 4) == "#39;") { i += 4; out += '\''; }
			else if (in.mid(i, 4) == "#60;") { i += 4; out += '<'; }
			else if (in.mid(i, 4) == "#62;") { i += 4; out += '>'; }
			else if (in.mid(i, 5) == "#160;") { i += 5; out += ' '; }
		}
		else {
			out += in[i];
			i++;
		}
	}
	return out;
}

QString TmlNode::encodeString(const QString& in) {
	QString out;
	int i = 0;
	while (i < in.size()) {
		if (in[i] == "<") {
			out += "&lt";
		} else if (in[i] == ">") {
			out += "&gt";
		} else if (in[i] == "&") {
			out += "&amp";
		} else if (in[i] == "'") {
			out += "&apos";
		}
		else if (in[i] == "\"") {
			out += "&quot";
		} else {
			out += in[i];
		}
		i++;
	}

	return out;
}

void TmlNode::exportNode(QString& str, TmlNode* node, TmlNode* root, int max_line_size) {
	QString line;

	line = genSpaces(nodeDephth(node, root) * 2);
	line += "<";
	line += node->name;
	line += " ";

	for (int i = 0; i < node->args.size(); i++) {

		QString arg_data = node->args[i]->getName();
		arg_data += "=\"";
		arg_data += encodeString(node->args[i]->getValue());
		arg_data += "\" ";

		if (line.size() + arg_data.size() + 2 > max_line_size) 
		{
			str += line;
			str += "\r\n";
			line = genSpaces(nodeDephth(node, root) * 2 + 2);
			line += arg_data;
		}
		else {
			line += arg_data;
		}
	}

	if (node->empty_element) {
		line += "/>";
		str += line;
		str += "\r\n";
		return;
	}

	line += ">";
	str += line;
	str += "\r\n";

	if (node->childs.size() == 0) {
		if (node->data != "") {
			str += genSpaces(nodeDephth(node, root) * 2 + 2);
			str += encodeString(node->data);
			str += "\r\n";
		}
	}
	else {
		for (int i = 0; i < node->childs.size(); i++) {
			exportNode(str, node->childs[i], root, max_line_size);
		}
	}

	str += "</";
	str += node->name;
	str += ">";
	str += "\r\n";
}

/*
int TmlNode::setBoolArg(const char* name, bool value) {

}

bool TmlNode::getBoolArg(const char* name) const {

}

int TmlNode::setIntArg(const char* name, int value);
int TmlNode::getIntArg(const char* name) const;

int TmlNode::setFloatArg(const char* name, float value);
float TmlNode::getFloatArg(const char* name) const;

int TmlNode::setDoubleArg(const char* name, double value);
double TmlNode::getDoubleArg(const char* name) const;

*/

QString TmlNode::toString(int max_line_size) {
	QString result;
	exportNode(result, this, this, max_line_size);
	return result;
}

QChar TmlStringSource::next() {
	src_pos++;
	
	if (ch == EOL) {
		pos = 0;
		line++;
	}
	else if (ch != EOS) {
		pos++;
	}

	if (src_pos >= src_size) {
		ch = EOS;
		return EOS;
	}

	if (src[src_pos] == '\r') {
		if (src_pos < src_size - 1) {
			if (src[src_pos + 1] == '\n') {
				src_pos++;
				ch = EOL;
				return EOL;
			}
		}
	}

	ch = src[src_pos];
	return ch;
}

QChar TmlStringSource::forward() {
	if (src_pos >= src_size)
		return EOS;
	return src[src_pos + 1];
}


void testTml(QString& path) {
	QDir dir(path);
	
	// Test1
	TmlNode* node = new TmlNode();
	node->setName("TestNode");
	node->setArgValue("arg1", "str1");
	node->setArgValue("arg2", "str2");

	TmlNode* child1 = node->createChild("Child1");
	child1->setArgValue("arg1", "str1");
	

	TmlNode* child2 = node->createChild("Child2");
	child2->setArgValue("arg1", "str1");
	child2->setEmptyElement(true);

	QString str1 = node->toString(40);

	TmlNode* node2 = new TmlNode();
	if (!node2->parse(str1)) {
		qDebug("Test1 parse error");
	}

	if (node2->getNumArgs() != 2) {
		qDebug("Test1 number of TestNode args failed");
	}

	if (!node2->isArg("arg1")) {
		qDebug("Test1 TestNode have not got arg1 arg");
	}

	if (!node2->isArg("arg2")) {
		qDebug("Test1 TestNode have not got arg2 arg");
	}

	TmlArgument* arg1 = node2->getArg("arg1");

	if (arg1 == NULL) {
		qDebug("Test1 TestNode getArg(\"arg1\") return NULL");
	}
	else {
		if (arg1->getValue() != QString("str1")) {
			qDebug("Test1 TestNode arg1 == \"str1\" failed");
		}
	}

	TmlArgument* arg2 = node2->getArg("arg2");

	if (arg2 == NULL) {
		qDebug("Test1 TestNode getArg(\"arg2\") return NULL");
	}
	else {
		if (arg2->getValue() != QString("str2")) {
			qDebug("Test1 TestNode arg2 == \"str2\" failed");
		}
	}



	if (node2->getNumChilds() != 2) {
		qDebug("Test1 number of TestNode childs failed");
	}

}


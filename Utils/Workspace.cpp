#include "Workspace.h"
#include "Core/Utils/Log.h"

#include <QScriptEngine>
#include "Core/Utils/Calendar.h"
#include "Core/Utils/QStringUtils.h"
#include <QDebug>


#include "Core/Nodes/NodeBase.h"
#include "Core/Nodes/NodeInclude.h"
#include "Core/Nodes/NodeVar.h"
#include "Core/Nodes/Pim/NodeEvent.h"
#include "Core/Nodes/Pim/NodePerson.h"
#include "Core/Nodes/Pim/NodeProject.h"
#include "Core/Nodes/Pim/NodeTask.h"


void Workspace::init(QString _dir) {
	QDir di = QDir(_dir);

	if (!di.exists()) {
		Log::get()->error("Workspace::init dir not exist");
		return;
	}

	dir = di;

	QString workscpace_file_name = dir.absoluteFilePath("workspace.xml");

	QFileInfo workspace_file(workscpace_file_name);

	if (!workspace_file.exists()) {
		Log::get()->error("Workspace::init workspace file not exist");
		return;
	}

	pushInputFile("workspace.xml");
	loadTml(NULL);

	Vector<NodeEvent*> events = Calendar::get()->getEvents(2022, 06, 16);

	for (int i = 0; i < events.size(); i++) {
		Log::get()->message("Event %s", events[i]->getStart().toString().toLocal8Bit().data());
	}

	/*
	QScriptEngine engine;

	QString str;
	bool load = QStringUtils::loadStringFromFileWindows(QString("D:/Workspace/test.js"), str);

	engine.evaluate(str);

	QScriptValue global = engine.globalObject();

	QScriptValue testFunc = global.property("test_func");

	if (testFunc.isFunction()) {
		QScriptValueList args;
		args << "hello";

		qDebug() << testFunc.call(QScriptValue(), args).toString(); // 27
	}
	else {
		qDebug() << "Nie jest funkcja";
	}
	*/
}

void Workspace::loadTml(NodeBase* parent) {
	
	InputFile* inFile = getTopInputFile();

	QString node_name = inFile->current_node->getName();

	NodeBase* node = createNode(node_name);

	if (node == NULL) {
		Log::get()->error("Workspace::loadTml can not create node : %s", node_name.toUtf8().data());
		return;
	}
	else {

		if (parent) {
			parent->addChild(node);
		}
		else {
			root = node;
		}

		node->LoadTml(inFile->current_node);
	}

	if (inFile != getTopInputFile()) {
		loadTml(node);
		popInputFile();
	}
	else {
		TmlNode* tml_node = inFile->current_node;
		for (int i = 0; i < tml_node->getNumChilds(); i++) {
			TmlNode* child = tml_node->getChild(i);
			
			if ((child->getName().left(6) == "param_") || (child->getName().left(5) == "prop_")) {
			
			}
			else {
				inFile->current_node = child;
				loadTml(node);
			}
		}
	}
}

NodeBase* Workspace::createNode(QString& name) {
	NodeBase* result = NULL;

	if (name == "dummy") {
		result = new NodeBase();
	}
	else if (name == "include") {
		result = new NodeInclude();
	}
	else if (name == "var") {
		result = new NodeVar();
	}
	else if (name == "event") {
		result = new NodeEvent();
	}
	else if (name == "person") {
		result = new NodePerson();
	}
	else if (name == "project") {
		result = new NodeProject();
	}
	else if (name == "task") {
		result = new NodeTask();
	}

	return result;
}

void Workspace::setVar(QString& name, QString& value) {
	vars[name] = value;
}

QString Workspace::getVar(QString& name) {
	if (vars.contains(name)) {
		return vars[name];
	}
	return "";
}

void Workspace::pushInputFile(QString relative_file_path) {
	
	QString file_name;

	if (inputFiles.size() == 0) {
		file_name = dir.absoluteFilePath(relative_file_path);
	}
	else {
		InputFile* in_file = getTopInputFile();
		QDir in_dir = in_file->file.dir();
		file_name = in_dir.absoluteFilePath(relative_file_path);
	}

	QFileInfo fi = QFileInfo(file_name);
	if (!fi.exists()) {
		Log::get()->error("Workspace::pushInputFile file not exist");
		return;
	}

	TmlNode* root_node = new TmlNode();

	QString src;
	bool parse_error;
	int parse_error_line, parse_error_pos;

	if (QStringUtils::loadStringFromFileWindows(fi.absoluteFilePath(), src)) {
		TmlStringSource* source = new TmlStringSource(src);
		root_node->parse(source);
		parse_error = source->error;
		parse_error_line = source->error_line;
		parse_error_pos = source->error_pos;
		delete source;
	}
	else {
		Log::get()->error("Workspace::pushInputFile file load error");
		return;
	}

	if (parse_error) {
		delete root_node;
		root_node = NULL;

		Log::get()->error("Workspace::pushInputFile file parse error %s %i %i", 
			fi.absoluteFilePath().toLocal8Bit().data(), parse_error_line, parse_error_pos);
		return;
	}

	InputFile* in_file = new InputFile();
	in_file->file = fi;
	in_file->root_node = root_node;
	in_file->current_node = root_node;

	inputFiles.append(in_file);
}

void Workspace::popInputFile() {
	if (inputFiles.size() > 0) {
		InputFile* ifile = inputFiles[inputFiles.size() - 1];
		delete ifile->root_node;
		ifile->current_node = NULL;
		ifile->root_node = NULL;
		delete ifile;
		inputFiles.remove(inputFiles.size() - 1);
	}
	else {
		Log::get()->error("Workspace::popInputFile inputFiles.size() < 1");
	}
}

const char* Workspace::getCorrentInputFile() {
	if (inputFiles.size() == 0) {
		return "";
	}
	else {
		InputFile* ifile = inputFiles[inputFiles.size() - 1];
		return dir.relativeFilePath(ifile->file.absolutePath()).toLocal8Bit().data();
	}
}


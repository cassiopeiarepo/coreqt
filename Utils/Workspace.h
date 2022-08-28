#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include "core/types/Base.h"
#include "core/utils/Singleton.h"
#include "core/types/Vector.h"
#include "coreqt/Common/Tml.h"

#include<QFileInfo>
#include <QMap>
#include <QDir>

class NodeBase;

class InputFile {
public:
	QFileInfo file;
	TmlNode* root_node;
	TmlNode* current_node;
};


class Workspace : public Singleton<Workspace> {
public:

	void init(QString _dir);

	const QDir& getDir() { return dir; }

	void setVar(QString& name, QString& value);
	QString getVar(QString& name);

	Vector<InputFile*>& getInputFile() { return inputFiles; }
	InputFile* getTopInputFile() {  if (inputFiles.size() == 0) 
										return NULL; 
									else return inputFiles[inputFiles.size() - 1];  }

	const char* getCorrentInputFile();


	void pushInputFile(QString relative_file_path);
	void popInputFile();

	NodeBase* createNode(QString& name);

private:
	QDir dir;
	QMap<QString, QString> vars;
	
	NodeBase* root;
	Vector<InputFile*> inputFiles;

	void loadTml(NodeBase* parent);

};


#endif /* __WORKSPACE_H__ */

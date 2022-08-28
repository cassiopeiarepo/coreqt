#include "TmlPreprocessor.h"
#include "app/Utils/Log.h"
#include "coreqt/Utils/QStringUtils.h"

void TmlPreprocessor::parseFile(const QString& file_path, const QString& file_type) {
	QFileInfo fi(file_path);

	if (!fi.exists()) {
		Log::get()->error("TmlPreprocessor:parseFile - file not exist");
		return;
	}

	if (file_type == "c") {
		parse_c(fi);
	}
	else {
		Log::get()->error("TmlPreprocessor:parseFile - unknown file type");
		return;
	}
}

void TmlPreprocessor::parseDir(const QString& dir_path, const QString& file_type) {

}

QChar TmlPreprocessor::next() {
	str_pos++;

	if (ch == TmlSource::EOL) {
		pos = 0;
		line++;
	}
	else if (ch != TmlSource::EOS) {
		pos++;
	}

	if (str_pos >= src_size) {
		ch = TmlSource::EOS;
		return TmlSource::EOS;
	}

	ch = input[str_pos];
	return ch;
}

QChar TmlPreprocessor::forward() {
	if (str_pos >= src_size)
		return TmlSource::EOS;
	return input[str_pos + 1];
}

void TmlPreprocessor::parse_c(const QFileInfo& fi) {

	input = "";
	if (!QStringUtils::loadStringFromFileUnix(fi.absoluteFilePath(), input)) {
		Log::get()->error("TmlPreprocessor:parse_c - file not load failed");
		return;
	}

	str_pos = -1;
	src_size = input.size();
	line = 1;
	pos = 1;
	ch = ' ';

	next();
	
	while (ch != TmlSource::EOS && ch != '/')
		next();

	if (ch == '/') {
		next();
		if (ch == '/') {
			QString line_comment;

			next();

			while (ch != TmlSource::EOS && ch != TmlSource::EOL) {
				line_comment += ch;
				next();
			}

		} else if (ch == '*') {
			QString multi_line_comment;

			next();

			//while (ch != TmlSource::EOS && )
		}
	}

}

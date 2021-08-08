#include "file.h"
#include "player_exception.h"

#include <vector>
#include <map>
#include <string>

using std::vector;
using std::string;

namespace {
	vector<string> split(const string &text, char delimiter, size_t limit) {
		vector<string> result;
		string current;

		for (char c : text) {
			if (c == delimiter && result.size() + 1 != limit) {
				result.push_back(current);
				current.clear();
			} else {
				current.push_back(c);
			}
		}

		result.push_back(current);
		return result;
	}
}

File::File(const string &description) {
	vector<string> items = split(description, '|', 0);
	if (items.size() < 2) {
		throw CorruptFileException();
	}

	const size_t itemCount = items.size();
	type = items[0];
	content = items[itemCount - 1];

	for (size_t i = 1; i + 1 < itemCount; i++) {
		vector<string> fields = split(items[i], ':', 2);
		if (fields.size() < 2) {
			throw InvalidMetadataException();
		}

		metadata[fields[0]] = fields[1];
	}
}

const string &File::getType() const {
	return type;
}

const File::metadata_t &File::getMetadata() const {
	return metadata;
}

const string &File::getContent() const {
	return content;
}

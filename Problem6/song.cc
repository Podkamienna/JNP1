#include "song.h"
#include "file.h"
#include "player_exception.h"

#include <iostream>
#include <regex>

using std::cout;
using std::regex;
using std::endl;

const regex SONG_REGEX(R"(^[0-9a-zA-Z\s,.!?':;-]*$)");

Song::Song(const File &file) {
	File::metadata_t metadata = file.getMetadata();

	auto titleIterator = metadata.find("title");
	if (titleIterator == metadata.end()) {
		throw InvalidMetadataException();
	}

	auto artistIterator = metadata.find("artist");
	if (artistIterator == metadata.end()) {
		throw InvalidMetadataException();
	}

	title = titleIterator->second;
	artist = artistIterator->second;

	content = file.getContent();

	if (!regex_match(content, SONG_REGEX)) {
		throw CorruptContentException();
	}
}

void Song::play() const {
	cout << "Song ";
	cout << "[" << artist << ", ";
	cout << title << "]: ";
	cout << content << endl;
}

bool Song::contains(PlaylistElement *playlistElement) const {
	return this == playlistElement;
}
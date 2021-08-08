#include "movie.h"
#include "playlist_element.h"
#include "file.h"
#include "player_exception.h"

#include <regex>
#include <iostream>
#include <string>

using std::cout;
using std::regex;
using std::endl;

const regex SONG_REGEX(R"(^[0-9a-zA-Z\s,.!?':;-]*$)");
const regex YEAR_REGEX(R"(^[-]?[1-9][0-9]*$)");

const char Movie::rot13[ALPHABET_SIZE] = {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1, 2, 3, 4, 5, 6, 7, 8,
                                          9, 10, 11, 12};

Movie::Movie(const File &file) {
	File::metadata_t metadata = file.getMetadata();

	auto titleIterator = metadata.find("title");
	if (titleIterator == metadata.end()) {
		throw InvalidMetadataException();
	}

	auto yearIterator = metadata.find("year");
	if (yearIterator == metadata.end()) {
		throw InvalidMetadataException();
	}

	title = titleIterator->second;
	year = yearIterator->second;

	content = file.getContent();

	if (!regex_match(year, YEAR_REGEX)) {
		throw InvalidMetadataException();
	}

	if (!regex_match(content, SONG_REGEX)) {
		throw CorruptContentException();
	}

	decipherROT13();
}

void Movie::play() const {
	cout << "Movie ";
	cout << "[" << title << ", ";
	cout << year << "]: ";
	cout << content << endl;
}

bool Movie::contains(PlaylistElement *playlistElement) const {
	return this == playlistElement;
}


void Movie::decipherROT13() {
	for (char &letter : content) {
		if ('a' <= letter && letter <= 'z') {
			letter = char('a' + rot13[letter - 'a']);
		}

		if ('A' <= letter && letter <= 'Z') {
			letter = char('A' + rot13[letter - 'A']);
		}
	}
}

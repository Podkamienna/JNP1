#include "playlist.h"
#include "play_mode.h"
#include "playlist_element.h"
#include "player_exception.h"

#include <cstddef>
#include <vector>
#include <iostream>
#include <memory>

using std::shared_ptr;
using std::cout;
using std::endl;


Playlist::Playlist(std::string_view playlistName) : name(playlistName), playMode(createSequenceMode()){}

void Playlist::add(const shared_ptr<PlaylistElement> &playlistElement) {
	add(playlistElement, playlist.size());
}

void Playlist::add(const shared_ptr<PlaylistElement> &playlistElement, size_t position) {
	if (position > playlist.size()) {
		throw OutOfBoundsException();
	}
	if (playlistElement->contains(this)) {
		throw LoopException();
	}

	playlist.insert(playlist.begin() + position, playlistElement);
}

void Playlist::remove() {
	if (playlist.empty()) {
		throw OutOfBoundsException();
	}

	remove(playlist.size() - 1);
}

void Playlist::remove(size_t position) {
	if (position >= playlist.size()) {
		throw OutOfBoundsException();
	}

	playlist.erase(playlist.begin() + position);
}

void Playlist::setMode(const shared_ptr<PlayMode> &newPlayMode) {
	playMode = newPlayMode;
}

void Playlist::play() const {
	cout << "Playlist [" << name << "]" << endl;

	playMode->play(playlist);
}

bool Playlist::contains(PlaylistElement *playlistElement) const {
	if (this == playlistElement) {
		return true;
	}

	for (const auto &element : playlist) {
		if (element->contains(playlistElement)) {
			return true;
		}
	}

	return false;
}
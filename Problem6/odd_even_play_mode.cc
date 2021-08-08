#include "odd_even_play_mode.h"
#include "playlist_element.h"

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

void OddEvenPlayMode::play(const vector<shared_ptr<PlaylistElement>> &playlist) {
	for (size_t i = 1; i < playlist.size(); i += 2) {
		playlist[i]->play();
	}

	for (size_t i = 0; i < playlist.size(); i += 2) {
		playlist[i]->play();
	}
}

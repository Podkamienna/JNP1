#include "shuffle_play_mode.h"
#include "playlist_element.h"

#include <memory>
#include <vector>
#include <random>
#include <algorithm>

using std::vector;
using std::iota;
using std::shared_ptr;

void ShufflePlayMode::play(const vector<shared_ptr<PlaylistElement>> &playlist) {
	vector<int> indices(playlist.size());
	iota(indices.begin(), indices.end(), 0);
	shuffle(indices.begin(), indices.end(), randomEngine);

	for (auto i : indices) {
		playlist[i]->play();
	}
}

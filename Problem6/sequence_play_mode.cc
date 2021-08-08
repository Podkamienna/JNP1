#include "sequence_play_mode.h"
#include "playlist_element.h"

#include <memory>
#include <vector>


using std::vector;
using std::shared_ptr;

void SequencePlayMode::play(const vector<shared_ptr<PlaylistElement>> &playlist) {
	for (const auto &ptr : playlist) {
		ptr->play();
	}
}

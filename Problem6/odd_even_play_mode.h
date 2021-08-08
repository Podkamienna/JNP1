#ifndef PLAYLIST_ODD_EVEN_PLAY_MODE_H
#define PLAYLIST_ODD_EVEN_PLAY_MODE_H

#include "play_mode.h"
#include "playlist_element.h"

#include <vector>
#include <memory>

class OddEvenPlayMode : public PlayMode {
public:
	void play(const std::vector<std::shared_ptr<PlaylistElement>> &playlist) override;
};


#endif //PLAYLIST_ODD_EVEN_PLAY_MODE_H

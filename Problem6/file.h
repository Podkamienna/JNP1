#ifndef PLAYLIST_FILE_H
#define PLAYLIST_FILE_H

#include <map>
#include <string>

class File {
public:
	using metadata_t = std::map<std::string, std::string>;

	explicit File(const std::string &description);

	[[nodiscard]] const std::string &getType() const;

	[[nodiscard]] const metadata_t &getMetadata() const;

	[[nodiscard]] const std::string &getContent() const;

private:
	std::string type;

	std::string content;

	metadata_t metadata;
};


#endif //PLAYLIST_FILE_H

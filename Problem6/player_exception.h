#ifndef PLAYLIST_PLAYER_EXCEPTION_H
#define PLAYLIST_PLAYER_EXCEPTION_H

#include <exception>

class PlayerException : public std::exception {
public:
	~PlayerException() override = default;

	[[nodiscard]] const char *what() const noexcept override = 0;
};

class UnsupportedTypeException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};

class InvalidMetadataException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};

class LoopException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};

class CorruptFileException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};

class CorruptContentException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};

class OutOfBoundsException : public PlayerException {
public:
	[[nodiscard]] const char *what() const noexcept override;
};



#endif //PLAYLIST_PLAYER_EXCEPTION_H

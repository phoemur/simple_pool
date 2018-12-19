#ifndef FONT_H
#define FONT_H

#include <SDL_ttf.h>
#include <string>

// SDL_ttf true type font wrapper
class Font {
    TTF_Font* gFont;

public:
    Font();
    Font(const std::string& path, int size);
    ~Font() noexcept;

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    Font(Font&&) noexcept = default;
    Font& operator=(Font&&) = default;

    TTF_Font* getFont() const noexcept {return gFont;}
    bool      isValid() const noexcept {return gFont != nullptr;}
    void loadFromFile(const std::string& path, int size);

private:
    void free() noexcept;
};

#endif // FONT_H

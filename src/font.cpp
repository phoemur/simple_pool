#include "font.h"
#include "exception.h"

Font::Font()
    : gFont{nullptr} {}

Font::Font(const std::string& path, int size)
    : Font()
{
    loadFromFile(path, size);
}


Font::~Font() noexcept
{
    free();
}

void Font::loadFromFile(const std::string& path, int size)
{
    free();

    gFont = TTF_OpenFont( path.c_str(), size );
    if (gFont == nullptr)
        throw SDLException(std::string("Failed to load font! SDL_ttf Error: ") + TTF_GetError());
}

void Font::free() noexcept
{
    if (gFont != nullptr)
        TTF_CloseFont( gFont );
}

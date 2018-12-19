// Visit http://lazyfoo.net/tutorials/SDL/index.php
#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>
#include <string>

// Forward declarations
class Font;

//Texture wrapper class
class Texture {
public:
    Texture();
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept = default;
    Texture& operator=(Texture&&) noexcept = default;

    bool loadFromFile( const std::string& path );
    bool loadFromRenderedText(const std::string& textureText, Font& f, SDL_Color textColor );
    void free();
    void setColor( Uint8 red, Uint8 green, Uint8 blue );
    void setBlendMode( SDL_BlendMode blending );
    void setAlpha( Uint8 alpha );
    void render( int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE );
    double  getWidth() const noexcept {return mWidth; }
    double getHeight() const noexcept {return mHeight;}

private:
    SDL_Texture* mTexture;

    double mWidth = 0;
    double mHeight = 0;
};

#endif // TEXTURE_H

#include "texture.h"
#include "font.h"
#include "mainwindow.h"
#include "exception.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

Texture::Texture()
    : mTexture{nullptr},
      mWidth{0}, mHeight{0}
{
    if (mainwindow == nullptr)
        throw SDLException("Could not locate MainWindow");
}

Texture::~Texture()
{
    free();
}

bool Texture::loadFromFile( const std::string& path )
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = nullptr;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == nullptr )
        throw SDLException(std::string("Unable to load image ") + path + " : " + IMG_GetError());

    //Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( mainwindow->getRenderer(), loadedSurface );
    if( newTexture == nullptr )
        throw SDLException(std::string("Unable to create texture from ") + path + " : " + SDL_GetError());

    //Get image dimensions
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );

    //Return success
    mTexture = newTexture;
    return mTexture != nullptr;
}

bool Texture::loadFromRenderedText( const std::string& textureText, Font& f, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( f.getFont(), textureText.c_str(), textColor );
    if( textSurface == nullptr )
        throw SDLException(std::string("Unable to render text surface! SDL_ttf Error: ") + TTF_GetError());

    //Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface( mainwindow->getRenderer(), textSurface );
    if( mTexture == nullptr )
        throw SDLException(std::string("Unable to create texture from text :") + SDL_GetError());

    //Get image dimensions
    mWidth = textSurface->w;
    mHeight = textSurface->h;

    //Get rid of old surface
    SDL_FreeSurface( textSurface );

    //Return success
    return mTexture != nullptr;
}

void Texture::free()
{
    //Free texture if it exists
    if( mTexture != nullptr )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
    //Set blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}

void Texture::setAlpha( Uint8 alpha )
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

void Texture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, static_cast<int>(mWidth), static_cast<int>(mHeight) };

    //Set clip rendering dimensions
    if( clip != nullptr )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx( mainwindow->getRenderer(), mTexture, clip, &renderQuad, angle, center, flip );
}

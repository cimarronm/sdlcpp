//
//  sdl.hh
//
//  A partial C++ wrapper around SDL2
//  Created by Cimarron Mittelsteadt
//

#ifndef sdl_hh
#define sdl_hh

#include <filesystem>
#include <stdexcept>
#include <optional>

#include <SDL.h>
#include <SDL_ttf.h>

namespace SDL
{
class Window
{
    friend class Renderer;
public:
    Window(const std::string_view title, int x, int y, int w, int h, Uint32 flags)
    {
        window = SDL_CreateWindow(title.data(), x, y, w, h, flags);
        if (!window) throw std::runtime_error("Could not create window");
    }
    ~Window()
    {
        if (window) SDL_DestroyWindow(window);
    }
private:
    SDL_Window* window;
};

class Renderer
{
    friend class Texture;
    friend class Font;
public:
    Renderer(const SDL::Window& window, int index, Uint32 flags)
    {
        renderer = SDL_CreateRenderer(window.window, index, flags);
        if (!renderer) throw std::runtime_error("Could not create renderer");
    }
    ~Renderer()
    {
        if (renderer) SDL_DestroyRenderer(renderer);
    }
    auto SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
    {
        return SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
    auto Clear() const
    {
        return SDL_RenderClear(renderer);
    }
    auto DrawLineF(float x1, float y1, float x2, float y2) const
    {
        return SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
    }
    auto Present() const
    {
        return SDL_RenderPresent(renderer);
    }
    
private:
    SDL_Renderer *renderer;
};

class Texture
{
public:
    Texture(const SDL::Renderer& renderer, Uint32 format, int access, int w, int h)
    {
        texture = SDL_CreateTexture(renderer.renderer, format, access, w, h);
        if (!texture) throw std::runtime_error("Could not create texture");
    }
    Texture(SDL_Texture* texture) : texture(texture) {};
    ~Texture()
    {
        if (texture) SDL_DestroyTexture(texture);
    }
    SDL_Rect getRect() const
    {
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return {.x=0, .y=0, .w=width, .h=height};
    }
    auto render(const Renderer& renderer, const std::optional<SDL_Rect>& srcrect,
               const std::optional<SDL_Rect>& dstrect) const
    {
        return SDL_RenderCopy(renderer.renderer, texture, (srcrect) ? &srcrect.value() : nullptr,
                              (dstrect) ? &dstrect.value() : nullptr);
    }
    
private:
    SDL_Texture* texture;
};

class Font
{
public:
    Font(const std::filesystem::path& fontFile, int ptsize)
    {
        font = TTF_OpenFont(fontFile.c_str(), ptsize);
        if (!font) throw std::runtime_error("Could not open font "+fontFile.string());
    }
    ~Font()
    {
        if (font) TTF_CloseFont(font);
    }
    auto renderTexture(const Renderer& renderer, const std::string_view text, SDL_Color fg) const
    {
        auto surf = TTF_RenderText_Blended(font, text.data(), fg);
        auto texture = SDL::Texture(SDL_CreateTextureFromSurface(renderer.renderer, surf));
        SDL_FreeSurface(surf);
        return texture;
    }
private:
    TTF_Font* font;
};

}

#endif /* sdl_hh */

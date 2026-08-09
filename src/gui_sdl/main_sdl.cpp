#if defined(_WIN32)
// Placeholder: SDL GUI primarily targets Linux in this repo.
#endif

#include "operator/expression_evaluator.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <locale>
#include <codecvt>

using namespace calculator;

static std::wstring ToWString(const std::string& s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(s);
}

static std::string ToString(const std::wstring& ws) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(ws);
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init Error: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("CalcCpp SDL", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (!win) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        SDL_Log("CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        SDL_Log("OpenFont failed: %s", TTF_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    std::string input;
    bool running = true;
    SDL_StartTextInput();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_TEXTINPUT) {
                input += e.text.text;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !input.empty()) {
                    input.pop_back();
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    const std::wstring expr = ToWString(input);
                    const EvaluationResult r = ExpressionEvaluator::Evaluate(expr);
                    if (r.success) {
                        input = ToString(ExpressionEvaluator::FormatValue(r.value));
                    } else {
                        input = ToString(r.error);
                    }
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    input.clear();
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 30, 30, 30, 255);
        SDL_RenderClear(ren);

        // render input text
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, input.c_str(), color);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
            int w = surf->w;
            int h = surf->h;
            SDL_FreeSurface(surf);
            if (tex) {
                SDL_Rect dst = {10, 10, w, h};
                SDL_RenderCopy(ren, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

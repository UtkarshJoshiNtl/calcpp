// Button-based SDL GUI for CalcCpp (scientific calculator)

#include "operator/expression_evaluator.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <locale>
#include <codecvt>

using namespace calculator;

namespace {

const char* FindSystemFontPath() {
    static const char* const kCandidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/noto/NotoSans-Regular.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
    };

    for (const char* path : kCandidates) {
        if (std::ifstream(path).good()) {
            return path;
        }
    }
    return nullptr;
}

}  // namespace

static std::wstring ToWString(const std::string& s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(s);
}

static std::string ToString(const std::wstring& ws) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(ws);
}

struct Button {
    std::string label;
    SDL_Rect rect;
};

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

    const int winW = 480;
    const int winH = 640;
    SDL_Window* win = SDL_CreateWindow("CalcCpp - Scientific", 100, 100, winW, winH, SDL_WINDOW_SHOWN);
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

    const char* fontPath = FindSystemFontPath();
    if (fontPath == nullptr) {
        SDL_Log("No system font found; install fonts-dejavu-core or a TrueType font");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont(fontPath, 20);
    if (!font) {
        SDL_Log("OpenFont failed (%s): %s", fontPath, TTF_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    std::string input;
    bool running = true;

    // Layout: top display, button grid below
    const int displayH = 100;
    const int gridX = 10;
    const int gridY = displayH + 20;
    const int cols = 4;
    const int rows = 7;
    const int btnW = (winW - gridX*2 - (cols-1)*10) / cols;
    const int btnH = 60;

    std::vector<std::string> labels = {
        "sin","cos","tan","sqrt",
        "ln","log","pi","e",
        "(",")","^","/",
        "7","8","9","*",
        "4","5","6","-",
        "1","2","3","+",
        "0",".","C","="
    };

    std::vector<Button> buttons;
    buttons.reserve(labels.size());
    for (size_t i = 0; i < labels.size(); ++i) {
        int r = i / cols;
        int c = i % cols;
        SDL_Rect rc;
        rc.x = gridX + c * (btnW + 10);
        rc.y = gridY + r * (btnH + 10);
        rc.w = btnW;
        rc.h = btnH;
        buttons.push_back(Button{labels[i], rc});
    }

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                for (const auto& b : buttons) {
                    if (mx >= b.rect.x && mx <= b.rect.x + b.rect.w &&
                        my >= b.rect.y && my <= b.rect.y + b.rect.h) {
                        const std::string &L = b.label;
                        if (L == "=") {
                            const std::wstring expr = ToWString(input);
                            const EvaluationResult r = ExpressionEvaluator::Evaluate(expr);
                            if (r.success) {
                                input = ToString(ExpressionEvaluator::FormatValue(r.value));
                            } else {
                                input = ToString(r.error);
                            }
                        } else if (L == "C") {
                            input.clear();
                        } else if (L == "pi") {
                            input += "pi";
                        } else if (L == "e") {
                            input += "e";
                        } else if (L == "sin" || L == "cos" || L == "tan" || L == "ln" || L == "log" || L == "sqrt") {
                            input += L + std::string("(");
                        } else {
                            input += L;
                        }
                    }
                }
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
                    running = false;
                }
            }
        }

        // Render
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        // Display background
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_Rect disp = {0, 0, winW, displayH};
        SDL_RenderFillRect(ren, &disp);

        // Render input text right-aligned
        SDL_Color txtColor = {255,255,255,255};
        std::string displayStr = input.empty() ? "0" : input;
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, displayStr.c_str(), txtColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
            int w = surf->w;
            int h = surf->h;
            SDL_FreeSurface(surf);
            if (tex) {
                SDL_Rect dst = {winW - w - 10, 10, w, h};
                SDL_RenderCopy(ren, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
            }
        }

        // Draw buttons
        for (const auto& b : buttons) {
            SDL_SetRenderDrawColor(ren, 60, 60, 60, 255);
            SDL_RenderFillRect(ren, &b.rect);
            SDL_SetRenderDrawColor(ren, 90, 90, 90, 255);
            SDL_RenderDrawRect(ren, &b.rect);
            // render label centered
            SDL_Surface* s = TTF_RenderUTF8_Blended(font, b.label.c_str(), txtColor);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(ren, s);
                int tw = s->w;
                int th = s->h;
                SDL_FreeSurface(s);
                if (t) {
                    SDL_Rect dst = {b.rect.x + (b.rect.w - tw)/2, b.rect.y + (b.rect.h - th)/2, tw, th};
                    SDL_RenderCopy(ren, t, nullptr, &dst);
                    SDL_DestroyTexture(t);
                }
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

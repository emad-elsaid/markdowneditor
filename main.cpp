#include "raylib.h"
#include <fstream>
#include <vector>
#include "text.cpp"

using std::ifstream;
using std::vector;
using std::string;

struct Header {
  string prefix;
  int size;
  Font font;
};

Font font;

constexpr auto margin = 50;
constexpr auto blockMargin = 10;
constexpr auto headerMargin = 20;
constexpr auto fontsize = 18;
constexpr auto screenWidth = 1024;
constexpr auto screenHeight = 768;
Header headers[] = {
    {"######", static_cast<int>(fontsize * 1.2)},
    {"#####", static_cast<int>(fontsize * 1.4)},
    {"####", static_cast<int>(fontsize * 1.6)},
    {"###", static_cast<int>(fontsize * 1.8)},
    {"##", static_cast<int>(fontsize * 2.0)},
    {"#", static_cast<int>(fontsize * 2.2)},
};

int renderBlockquote(string line, int y) {
  Vector2 rect = {fontsize / 2.0 + margin, static_cast<float>(y + blockMargin)};
  auto height = fontsize + blockMargin * 2;

  auto firstChar = line.find_first_not_of("> \t");
  firstChar = firstChar < line.size() ? firstChar : 0;
  auto str = line.substr(firstChar);
  DrawRectangle(margin, y, GetScreenWidth() - margin * 2, height , LIGHTGRAY);
  DrawTextEx(font, str.c_str(), rect, fontsize, 0, DARKGRAY);

  return height;
}

int renderHeader(string line, int y) {
  y += headerMargin;
  for (auto &h : headers)
    if (line.starts_with(h.prefix)) {
      auto firstChar = line.find_first_not_of("# \t");
      firstChar = firstChar < line.size() ? firstChar : 0;
      auto str = line.substr(firstChar);
      DrawTextEx(h.font, str.c_str(), {margin, static_cast<float>(y)}, h.size, 0, BLACK);
      return h.size + headerMargin;
    }

  return 0;
}

int renderSeparator(string line, int y) {
  DrawLine(margin, y, GetScreenWidth() - margin, y, GRAY);
  return fontsize;
}

int renderParagraph(string line, int y) {
  Rectangle rect = {margin, static_cast<float>(y),
               static_cast<float>(GetScreenWidth() - (margin * 2)),
               0};
  return DrawTextBoxed(font, line.c_str(), rect, fontsize, 0, true, BLACK);
}

Font setupFont(const char* path, int size) {
  static const auto filter = TEXTURE_FILTER_ANISOTROPIC_16X;
  auto f = LoadFontEx(path, size, nullptr, 500);
  SetTextureFilter(font.texture, filter);
  return f;
}

int main(void) {
  ifstream input("TEST.md");
  auto content = vector<string>();
  for (string line; getline(input, line);)
    content.push_back(line);
  input.close();

  InitWindow(screenWidth, screenHeight, "Markdown viewer");

  font = setupFont("Inter/Inter-Regular.ttf", fontsize);
  for(auto& h : headers)
    h.font = setupFont("Inter/Inter-Bold.ttf", h.size);

  auto yOffset = 0;
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if(IsKeyPressed(KEY_Q)) break;

    yOffset += GetMouseWheelMove() * fontsize;
    if(yOffset<0) yOffset = 0;

    BeginDrawing();
    {
      ClearBackground(RAYWHITE);
      auto y = margin - yOffset;
      for(auto& line:content) {
        if (line == "---" || line == "___" || line == "***") {
          y += renderSeparator(line, y);
        } else if (line.starts_with("#")) {
          y += renderHeader(line, y);
        } else if (line.starts_with(">") || line.starts_with(">>") || line.starts_with("> > >")) {
          y += renderBlockquote(line, y);
        } else {
          y += renderParagraph(line, y);
        }
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

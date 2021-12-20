#include "raylib.h"
#include <fstream>
#include <vector>

using std::ifstream;
using std::vector;
using std::string;

struct Header {
  string prefix;
  int size;
};

Font font;
constexpr auto margin = 10;
constexpr auto fontsize = 18;
constexpr auto screenWidth = 800;
constexpr auto screenHeight = 600;
constexpr auto fontname = "Roboto/Roboto-Regular.ttf";
const Header headers[] = {
    {"######", static_cast<int>(fontsize * 1.2)},
    {"#####", static_cast<int>(fontsize * 1.4)},
    {"####", static_cast<int>(fontsize * 1.6)},
    {"###", static_cast<int>(fontsize * 1.8)},
    {"##", static_cast<int>(fontsize * 2.0)},
    {"#", static_cast<int>(fontsize * 2.2)},
};

int renderBlockquote(string line, int y) {
  Vector2 rect = {fontsize / 2.0 + margin, static_cast<float>(y + margin)};
  auto height = fontsize + margin * 2;

  DrawRectangle(margin, y, GetScreenWidth() - margin * 2, height , LIGHTGRAY);
  DrawTextEx(font, line.c_str(), rect, fontsize, 0, DARKGRAY);

  return height;
}

int renderHeader(string line, int y) {
  for (auto &h : headers)
    if (line.starts_with(h.prefix)) {
      DrawTextEx(font, line.substr(h.prefix.length()).c_str(), {margin, static_cast<float>(y)}, h.size, 0, BLACK);
      return h.size;
    }

  return 0;
}

int renderSeparator(string line, int y) {
  DrawLine(margin, y, screenWidth, y, GRAY);
  return fontsize;
}

int main(void) {
  ifstream input("README.md");
  auto content = vector<string>();
  for (string line; getline(input, line);)
    content.push_back(line);
  input.close();

  InitWindow(screenWidth, screenHeight, "Markdown viewer");

  font = LoadFont(fontname);
  auto yOffset = 0;
  SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

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
          DrawTextEx(font, line.c_str(), {margin, static_cast<float>(y)}, fontsize, 0, BLACK);
          y += fontsize;
        }
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

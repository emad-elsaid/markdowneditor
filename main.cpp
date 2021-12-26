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
  bool lineAfter;
  Font font;
};

Font font;
Font titleFont;

constexpr auto targetFPS = 120;
constexpr auto margin = 100;
constexpr auto blockMargin = 10;
constexpr auto headerMargin = 20;
constexpr auto separatorMargin = 20;
constexpr auto paragraphMargin = 20;
constexpr auto fontsize = 18;
constexpr auto screenWidth = 1024;
constexpr auto screenHeight = 768;
constexpr auto headerLineColor = LIGHTGRAY;
constexpr auto titleSize = fontsize * 2.5;
Header headers[] = {
  {"######", static_cast<int>(fontsize * 1.2), false},
  {"#####", static_cast<int>(fontsize * 1.4), false},
  {"####", static_cast<int>(fontsize * 1.6), false},
  {"###", static_cast<int>(fontsize * 1.8), false},
  {"##", static_cast<int>(fontsize * 2.0), true},
  {"#", static_cast<int>(fontsize * 2.2), true},
};

int renderTitle(string line, int y) {
  auto height = headerMargin;
  auto firstChar = line.find_first_not_of(" \t");
  firstChar = firstChar < line.size() ? firstChar : 0;
  auto str = line.substr(firstChar);

  DrawTextEx(titleFont, str.c_str(), {margin, static_cast<float>(y + height)},
             titleSize, 0, BLACK);
  height += titleSize;
  return height;
}

int renderTitleLine(string line, int y) {
  DrawTextEx(titleFont, line.c_str(), {margin, static_cast<float>(y)}, titleSize, 0, LIGHTGRAY);
  return titleSize + fontsize + headerMargin;
}

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
  auto height = headerMargin;

  for (auto &h : headers)
    if (line.starts_with(h.prefix)) {

      auto firstChar = line.find_first_not_of("# \t");
      firstChar = firstChar < line.size() ? firstChar : 0;
      auto str = line.substr(firstChar);
      DrawTextEx(h.font, str.c_str(), {margin, static_cast<float>(y+height)}, h.size, 0, BLACK);
      height += h.size;

      if(h.lineAfter) {
        height += fontsize;
        DrawLine(margin, y+height, GetScreenWidth() - margin, y+height, headerLineColor);
      }

      height += headerMargin;
      break;
    }

  return height;
}

int renderSeparator(string line, int y) {
  DrawLine(margin, y+separatorMargin, GetScreenWidth() - margin, y+separatorMargin, GRAY);
  return separatorMargin*2;
}

int renderParagraph(string line, int y) {
  Rectangle rect = {margin, static_cast<float>(y+paragraphMargin),
               static_cast<float>(GetScreenWidth() - (margin * 2)),
               0};
  return DrawTextBoxed(font, line.c_str(), rect, fontsize, 0, true, BLACK) + paragraphMargin;
}

int renderCodeBlock(vector<string>& block, int y) {
  auto offset = paragraphMargin;
  for(auto& line:block) {
    Rectangle rect = {margin, static_cast<float>(y + offset), static_cast<float>(GetScreenWidth() - (margin * 2)), 0};
    offset += DrawTextBoxed(font, line.c_str(), rect, fontsize, 0, false, ORANGE) + fontsize;
  }

  return offset;
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
  titleFont = setupFont("Inter/Inter-ExtraBold.ttf", titleSize);
  for(auto& h : headers)
    h.font = setupFont("Inter/Inter-Bold.ttf", h.size);

  auto yOffset = 0;
  SetTargetFPS(targetFPS);
  auto needsRender = true;

  while (!WindowShouldClose()) {
    if(IsKeyPressed(KEY_Q)) break;
    if(IsKeyDown(KEY_J)) {
      yOffset += fontsize;
      needsRender = true;
    }
    if(IsKeyDown(KEY_K)) {
      yOffset -= fontsize;
      needsRender = true;
    }
    if (GetMouseWheelMove()!=0) {
      yOffset += GetMouseWheelMove() * fontsize;
      needsRender = true;
    }
    if(yOffset<0) {
      yOffset = 0; // don't scroll past the beginning
      needsRender = true;
    }

    BeginDrawing();
    if (needsRender){
      needsRender = false;

      ClearBackground(RAYWHITE);
      auto y = margin - yOffset;
      for(auto iter = content.begin(); iter != content.end(); iter++) {
        auto line = *iter;
        auto hasNext = (iter+1) != content.end();
        auto nextLine = *(iter+1);
        if (line == "---" || line == "___" || line == "***") {
          y += renderSeparator(line, y);
        } else if (line.empty()) {
          // ignore empty lines
        } else if (line.starts_with("#")) {
          y += renderHeader(line, y);
        } else if (line.starts_with(">")) {
          y += renderBlockquote(line, y);
        } else if (hasNext && nextLine.starts_with("===") && nextLine.find_first_not_of("=") == string::npos) {
          y += renderTitle(line, y);
          y += renderTitleLine(nextLine, y);
          iter++;
        } else if (line.starts_with("```")) {
          auto block = vector<string>();
          block.push_back(line);
          while(++iter != content.end()) {
            block.push_back(*iter);
            if (*iter == "```") break;
          }
          y += renderCodeBlock(block, y);
        } else {
          y += renderParagraph(line, y);
        }
      }

      DrawFPS(10,10);

      if (y < GetScreenHeight() - margin) yOffset -= fontsize; // don't scroll past the end
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

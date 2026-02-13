#include "raylib.h"
#include "raymath.h"
#include <numbers>
#include <string>
#include <vector>

struct Tile {
  float width;
  float height;
  Vector2 position;
  Color color;
  Color BorderColor;
};

void DrawTile(Tile tile) {
  DrawRectangle(tile.position.x, tile.position.y, tile.width, tile.height,
                tile.color);

  DrawRectangleLines(tile.position.x, tile.position.y, tile.width, tile.height,
                     tile.BorderColor);
}

void UpdateTile(Tile *tile, Color newColor) { tile->color = newColor; }

struct Grid {
  std::vector<std::vector<Tile>> tiles;
  int rows;
  int cols;
};

Grid PixelGrid(int screenWidth, int screenHeight, float tilePixelSize) {
  Grid grid;
  grid.rows = screenHeight / tilePixelSize;
  grid.cols = screenWidth / tilePixelSize;

  for (int row = 0; row < grid.rows; row++) {
    std::vector<Tile> tileRow;
    for (int col = 0; col < grid.cols; col++) {
      Tile tile{tilePixelSize, tilePixelSize,
                Vector2{float(col * tilePixelSize), float(row * tilePixelSize)},
                WHITE, GRAY};
      tileRow.push_back(tile);
    }
    grid.tiles.push_back(tileRow);
  }
  return grid;
}

void DrawGrid(Grid &grid) {
  for (auto &row : grid.tiles) {
    for (auto &tile : row) {
      DrawTile(tile);
    }
  }
}

bool checkNeighbours(const Grid &grid, int row, int col) {
  int neighbours = 0;

  std::vector<std::pair<int, int>> directions = {
      {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};

  for (auto &direction : directions) {
    int newRow = row + direction.second;
    int newCol = col + direction.first;

    if (newCol >= 0 && newCol < grid.cols && newRow >= 0 &&
        newRow < grid.rows) {

      if (grid.tiles[newRow][newCol].color.r == 0) { // BLACK
        neighbours++;
      }
    }
  }

  if (grid.tiles[row][col].color.r == 255) { // WHITE
    return neighbours == 3;
  }
  return neighbours == 2 || neighbours == 3; // BLACK
}

void CellularAutomation(Grid &grid) {

  // temp is the new grid that will replace the old grid
  std::vector<std::vector<Tile>> temp;
  for (int row = 0; row < grid.rows; row++) {
    std::vector<Tile> tempRow;
    for (int col = 0; col < grid.cols; col++) {
      Tile &tile = grid.tiles[row][col];

      // update the states based on the neighbours
      if (checkNeighbours(grid, row, col)) {
        tempRow.push_back(
            {tile.width, tile.height, tile.position, BLACK, GRAY});
      } else {
        tempRow.push_back(
            {tile.width, tile.height, tile.position, WHITE, GRAY});
      }
    }
    temp.push_back(tempRow);
  }
  grid.tiles = temp;
}

void ClearGrid(Grid &grid) {
  for (auto &row : grid.tiles) {
    for (auto &tile : row) {
      tile.color = WHITE;
    }
  }
}

void RandomizeGrid(Grid &grid) {
  ClearGrid(grid);
  for (int row = 0; row < grid.rows; row++) {
    for (int col = 0; col < grid.cols; col++) {
      if (rand() % 2) {
        grid.tiles[row][col].color = BLACK;
      }
    }
  }
}

void UpdateGrid(Grid &grid, int row, int col, Color newColor) {
  if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols) {
    UpdateTile(&grid.tiles[row][col], newColor);
  }
}

int main() {
  constexpr int screenWidth = 800;
  constexpr int screenHeight = 600;
  constexpr int worldWidth = 1000;
  constexpr int worldHeight = 1000;
  InitWindow(screenWidth, screenHeight, "Game of Life");
  SetTargetFPS(60);

  Camera2D camera{};
  camera.target = (Vector2){worldWidth / 2.0f, worldHeight / 2.0f};
  camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  bool startCellularAutomation = false;
  int generation = 0;

  float tilePixelSize = 10;
  Grid grid = PixelGrid(worldWidth, worldHeight, tilePixelSize);

  Vector2 prevMousePos = GetMousePosition();
  float timer = 0.0f;

  float updateInterval = 1.0f;
  Clamp(updateInterval, 0.1f, 10.0f);

  while (!WindowShouldClose()) {

    timer += GetFrameTime();

    float mouseDelta = GetMouseWheelMove();
    float newZoom = camera.zoom + mouseDelta * 0.1f;
    if (newZoom <= 0) {
      newZoom = 0.1f;
    }
    camera.zoom = newZoom;

    Vector2 thisPos = GetMousePosition();
    Vector2 mouseDeltaPos = Vector2Subtract(thisPos, prevMousePos);
    prevMousePos = thisPos;


    if (IsKeyDown(KEY_UP)){
      updateInterval = updateInterval * 0.9f;
    }
    if (IsKeyDown(KEY_DOWN)){
      updateInterval = updateInterval / 0.9f;
    }

    updateInterval = Clamp(updateInterval, 0.01f, 10.0f);


    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      camera.target = Vector2Scale(mouseDeltaPos, -1.0f / camera.zoom);
      camera.offset = Vector2Add(camera.offset, mouseDeltaPos);
    }

    if (IsKeyPressed(KEY_SPACE)) {
      startCellularAutomation = !startCellularAutomation;
    }

    if (IsKeyPressed(KEY_R) and !startCellularAutomation) {
      RandomizeGrid(grid);
      generation = 0;
    }

    if (IsKeyPressed(KEY_C) and !startCellularAutomation) {
      ClearGrid(grid);
      generation = 0;
    }

    if (timer > updateInterval && startCellularAutomation) {
      CellularAutomation(grid);
      timer = 0.0f;
      generation++;
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
      int col = mousePos.x / tilePixelSize;
      int row = mousePos.y / tilePixelSize;

      if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols) {
        UpdateGrid(grid, row, col, BLACK);
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);
    DrawGrid(grid);
    EndMode2D();

    DrawText(TextFormat("Generation: %d", generation), 10, 10, 20, BLACK);
    DrawText(TextFormat("Zoom: %.2f", camera.zoom), 10, 30, 20, BLACK);
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 50, 20, BLACK);
    DrawText(TextFormat("Update Interval: %.2f", updateInterval), 10, 70, 20, BLACK);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

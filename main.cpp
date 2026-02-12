#include "raylib.h"
#include <numbers>
#include <vector>
#include <string>

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
      Tile tile = {
          tilePixelSize, tilePixelSize,
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

bool checkNeighbours(const Grid& grid, int row, int col) {
    int neighbours = 0;

    std::vector<std::pair<int, int>> directions = {
        {0,1},{1,0},{0,-1},{-1,0},
        {1,1},{-1,-1},{-1,1},{1,-1}
    };

    for (auto& direction : directions) {
        int newRow = row + direction.second;
        int newCol = col + direction.first;

        if (newCol >= 0 && newCol < grid.cols &&
            newRow >= 0 && newRow < grid.rows) {

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
  std::vector<std::vector<Tile>> temp;
  for (int row = 0; row < grid.rows; row++) {
    std::vector<Tile> tempRow;
    for (int col = 0; col < grid.cols; col++) {
      Tile& tile = grid.tiles[row][col];
      if (checkNeighbours(grid, row, col)){
        tempRow.push_back({tile.width, tile.height, tile.position, BLACK, GRAY});
      }
      else {
        tempRow.push_back({tile.width, tile.height, tile.position, WHITE, GRAY});
      }
    }
    temp.push_back(tempRow);
  }
  grid.tiles = temp;
}


void UpdateGrid(Grid &grid, int row, int col, Color newColor) {
  if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols) {
    UpdateTile(&grid.tiles[row][col], newColor);
  }
}

int main() {
  constexpr int screenWidth = 800;
  constexpr int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Game of Life");
  SetTargetFPS(60);

  Camera2D camera{};
  camera.target = (Vector2){0.0f, 0.0f};
  camera.offset = (Vector2){0.0f, 0.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  bool startCellularAutomation = false;
  int generation = 0;

  float tilePixelSize = 10;
  Grid grid = PixelGrid(screenWidth, screenHeight, tilePixelSize);

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_SPACE)) {
      startCellularAutomation = !startCellularAutomation;
    }

    if (startCellularAutomation) {
      CellularAutomation(grid);
      generation++;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mousePos = GetMousePosition();
      int col = mousePos.x / tilePixelSize;
      int row = mousePos.y / tilePixelSize;

      if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols) {
        Color currentColor = grid.tiles[row][col].color;
        Color newColor = (currentColor.r == 255) ? BLACK : WHITE;
        UpdateGrid(grid, row, col, newColor);
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);
    DrawGrid(grid);
    DrawText(TextFormat("Generation: %d", generation), 10, 10, 20, BLACK);
    EndMode2D();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

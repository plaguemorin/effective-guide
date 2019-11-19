
#include <cstdio>

#include <Engine.hpp>
#include <Engine/ThingSystem/Thing.hpp>
#include <Engine/ThingSystem/Actor.hpp>
#include <Engine/ThingSystem/Enemy.hpp>
#include <Engine/ThingSystem/Manager.hpp>

class TextPrintComponent : public thing::ComponentT<TextPrintComponent> {
  char _begin, _end;
public:
  explicit TextPrintComponent(char a)
          : thing::ComponentT<TextPrintComponent>(),
            _begin(a), _end(a) {}

  TextPrintComponent(char b, char e)
          : thing::ComponentT<TextPrintComponent>(), _begin(b), _end(e) {}

  ~TextPrintComponent() override = default;

  [[nodiscard]] char begin() const { return _begin; }

  [[nodiscard]] char end() const { return _end; }
};

void print_map(const Map& map, const thing::Manager& manager) {
  for (int y = 0; y < map.height(); y++) {
    for (int x = 0; x < map.width(); x++) {
      char modStart = ' ';
      char modEnd = ' ';

      if (auto thing = manager.at(x, y)) {
        if (auto textprint = thing->get_component<TextPrintComponent>()) {
          modStart = textprint->begin();
          modEnd = textprint->end();
        }
      }

      printf("%c%04X%c",
             modStart,
             map.info_at_tile(x, y).tile_id,
             modEnd);
    }
    printf(" |\n");
  }
}

int main() {
  printf("Testing Maps...\n");
  Tileset aTileset(1784, 24, 24);

  auto aMap = Map::create_from_data(
          &aTileset, 20,
          280, 276, 276, 276, 290, 276, 276, 276, 276, 276, 276, 276, 290, 276, 276, 276, 276, 276, 276, 281, 278, 268, 268, 268, 268, 217, 217, 217, 217, 217,
          217, 217, 217, 217, 217, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 268, 268, 268, 268, 278, 278, 268, 268,
          268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 268,
          268, 268, 268, 278, 278, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268,
          268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268,
          268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268,
          268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268,
          268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278,
          268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268,
          268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 277, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 268, 268,
          268, 268, 268, 268, 278, 268, 268, 268, 268, 268, 268, 268, 278, 278, 268, 268, 268, 268, 280, 276, 276, 276, 276, 276, 283, 268, 268, 268, 268, 268, 268, 268, 278,
          278, 268, 268, 268, 268, 289, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 278, 282, 276, 276, 276, 276, 283, 269, 269, 269, 269, 269, 269, 269,
          275, 276, 276, 276, 276, 276, 283);

  thing::Thing aThing;
  thing::Actor aActor;
  thing::Enemy aEnemy;

  aThing.set_position(1, 1);
  aActor.set_position(2, 2);
  aEnemy.set_position(2, 3);

  aActor.set_name("PLAYER");
  aEnemy.set_name("ENEMY");

  thing::Manager manager;
  manager.register_object(&aThing);
  manager.register_object(&aActor);
  manager.register_object(&aEnemy);

  aThing.add_component<TextPrintComponent>('%');
  aActor.add_component<TextPrintComponent>('(', ')');
  aEnemy.add_component<TextPrintComponent>('>', '<');

  print_map(aMap, manager);
}

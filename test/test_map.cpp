
#include <cstdio>
#include <Engine.hpp>
#include <Engine/ThingSystem/Thing.hpp>
#include <Engine/ThingSystem/Actor.hpp>
#include <Engine/ThingSystem/Enemy.hpp>
#include <Engine/ThingSystem/Manager.hpp>


class TestComponent : public thing::ComponentT<TestComponent> {
public:
  void Test() const {
    printf("this is a working test!\n");
  }
};

int main() {
  printf("Testing Maps...\n");
  Tileset aTileset(4, 24, 24);
  aTileset[0].texture_id = 1;
  aTileset[1].texture_id = 1;
  aTileset[2].texture_id = 1;
  aTileset[3].texture_id = 1;

  Map aMap(5, 5, &aTileset);
  thing::Thing aThing;
  thing::Actor aActor;
  thing::Enemy aEnemy;

  aThing.set_position(1, 1);
  aActor.set_position(2, 2);
  aEnemy.set_position(2, 3);

  aActor.set_name("PLAYER");
  aEnemy.set_name("ENEMY");

  thing::Manager manager;
  manager.Register(&aThing);
  manager.Register(&aActor);
  manager.Register(&aEnemy);

  aThing.add_component<TestComponent>();
  aEnemy.add_component<TestComponent>();

  manager.WithAllThingsOfType<thing::Actor>([](thing::Actor *anActor) {
    printf("Actor %s\n", anActor->name());
  });

  manager.WithAllComponents<TestComponent>([](thing::Thing *aThing, TestComponent *cmp) {
    cmp->Test();
  });
}

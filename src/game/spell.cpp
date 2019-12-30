#include <lss/gameData.hpp>
#include "lss/game/spell.hpp"
// #include "lss/game/trigger.hpp"
#include "EventBus/EventBus.hpp"

void CellSpell::applyEffect(std::shared_ptr<Location> location,
                std::shared_ptr<Cell> c) {
  auto data = entt::service_locator<GameData>::get().lock();

if (spec == "FROSTBALL") {
    location->addTerrain(spec, c);
    // auto fb = std::make_shared<Terrain>(data->terrainSpecs[spec], 8);
    // fb->setCurrentCell(c);
    // location->addObject(fb);
    c->addFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
        n->addFeature(CellFeature::FROST);
    }
} else if (spec == "FIREBALL") {
    location->addTerrain(spec, c);
    // auto fb = std::make_shared<Terrain>(data->terrainSpecs[spec], 8);
    // fb->setCurrentCell(c);
    // location->addObject(fb);
    c->removeFeature(CellFeature::FROST);
    for (auto n : location->getNeighbors(c)) {
        n->removeFeature(CellFeature::FROST);
    }
} else if (spec == "ACIDPOOL") {
    location->addTerrain(spec, c);
    // auto fb = std::make_shared<Terrain>(data->terrainSpecs[spec], -5);
    // fb->setCurrentCell(c);
    // fb->triggers.push_back(std::make_shared<EnterTrigger>([=](std::shared_ptr<Creature> actor){
    //     return Triggers::ACID_POOL_TRIGGER(actor, location);
    // }));
    // location->addObject(fb);
    c->addFeature(CellFeature::ACID);
}

location->invalidate("apply spell effect");
}

void DamageSpell::applySpell(std::shared_ptr<Creature> caster,
                             std::shared_ptr<Location> location,
                             std::shared_ptr<Cell> c) {
  // auto objects = location->getObjects(c);
  // for (auto o : objects) {
  //   if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
  //     creature->applyDamage(
  //         std::dynamic_pointer_cast<Object>(location->player),
  //         damage.getDamage(caster->INTELLIGENCE(caster.get())));
  //   } else if (o->destructable && destroyObjects) {
  //     location->removeObject(o);
  //   }
  // }
  // if (c == location->player->currentCell) {
  //   location->player->applyDamage(
  //       std::dynamic_pointer_cast<Object>(location->player),
  //       damage.getDamage(caster->INTELLIGENCE(caster.get())));
  // }
  // applyEffect(location, c);
}

void DrillSpell::applySpell(std::shared_ptr<Creature> caster,
                            std::shared_ptr<Location> location,
                            std::shared_ptr<Cell> c) {
  // auto objects = location->getObjects(c);
  // for (auto o : objects) {
  //   location->removeObject(o);
  // }
  // DigEvent de(caster, c);
  // eb::EventBus::FireEvent(de);
  // applyEffect(location, c);
}

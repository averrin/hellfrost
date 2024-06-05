#pragma once
#include <entt/entt.hpp>
#include <string>

namespace lss {
class Action {
public:
  std::string name;
  int cost;
  int payed = 0;
  int start = 0;
  bool done = false;
  bool executed = false;

  entt::entity entity;
  std::vector<entt::entity> targets;

  std::function<void()> action;

  Action(){}
  Action(entt::entity entity, std::string name, int cost, std::function<void()> action)
      : entity(entity), name(name), cost(cost), action(action) {}
};
}

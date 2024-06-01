locations.DUNGEON = utils.makeLocation(
  "DUNGEON", CellType.FLOOR, CellType.WALL,
  {"DUNGEON"},
  100, 100,
  function(location)
    -- utils.placeRooms(location, 5, 10, 50, 50, 25, 25)
    utils.execTemplates(location, "DUNGEON", 0, 1)
    rooms = utils.placeRooms(location, 10, 30, 100, 100, 0, 0)
    utils.makePassages(location, rooms)
    utils.overrideFeatureProb(location, "CAVERN", "RIVER", p)
    utils.execTemplates(location, "CAVERN", 1, 2)
    utils.execTemplates(location, "DUNGEON", 1, 2)
    utils.fixOverlapped(location)
    utils.execTemplates(location, "DUNGEON", 2, 6)
    utils.placeWalls(location)
    utils.execTemplates(location, "DUNGEON", 6, 10)
    utils.placeStairs(location)
    utils.placeHero(location)
  end
);

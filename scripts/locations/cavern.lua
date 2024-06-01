locations.CAVERN = utils.makeLocation(
  "CAVERN", CellType.GROUND, CellType.WALL,
  {"CAVE"},
  100, 100,
  function(location)
    -- utils.placeRooms(location, 10, 30, 100, 100, 0, 0)
    utils.execTemplates(location, "CAVERN", 0, 1)
    utils.placeCaves(location, 10, 30, 100, 100, 0, 0)
    utils.execTemplates(location, "CAVERN", 1, 2)
    utils.fixOverlapped(location)
    utils.execTemplates(location, "CAVERN", 2, 6)
    utils.placeWalls(location)
    utils.execTemplates(location, "CAVERN", 6, 10)
    utils.cleanWalls(location)
    -- utils.makePassages(location)
    utils.placeStairs(location)
    utils.placeHero(location)
  end
);


locations.MIX = utils.makeLocation(
  "MIX", CellType.GROUND, CellType.WALL,
  {},
  160, 100,
  function(location)
    utils.execTemplates(location, "CAVERN", 0, 1)
    caves = utils.placeCaves(location, 10, 20, 80, 100, 0, 0)
    R1 = caves[Random.int(0, #caves - 1)]
    for _, r in ipairs(caves) do
      -- for _, c in ipairs(R1.cells) do
      --   c.tags:add("CAVE")
      -- end
      if (r.x + r.width > R1.x + R1.width) then
        R1 = r
        for _, c in ipairs(R1.cells) do
          if c == nil then goto continue end
          c.tags:add("CAVE")
          if c.type == CellType.GROUND then
            C1 = c
          end
          ::continue::
        end
      end
    end
    utils.execTemplates(location, "CAVERN", 1, 2)
    utils.fixOverlapped(location)
    utils.execTemplates(location, "CAVERN", 2, 6)
    utils.placeWalls(location)
    utils.execTemplates(location, "CAVERN", 6, 10)

    utils.cleanWalls(location)

    utils.execTemplates(location, "CAVERN", 0, 1)
    rooms = utils.placeRooms(location, 10, 20, 60, 100, 70, 0)
    R2 = rooms[Random.int(0, #rooms - 1)]
    for _, r in ipairs(rooms) do
      if (r.x + r.width < R2.x + R2.width) then
        R2 = r

        for _, c in ipairs(R2.cells) do
          if c == nil then goto continue end
          if c.type == CellType.FLOOR then
            C2 = c
            break
          end
          ::continue::
        end
      end
    end
    -- for _, r in ipairs(rooms) do
    -- r.tags:add("DUNGEON")
    --   for _, c in ipairs(r.cells) do
    --     c.tags:add("DUNGEON")
    --   end
    -- end
    print(C1, C2);
    if C1 ~= nil and C2 ~= nil then
      utils.makePassageBetweenCells(location,
        C1, C2
      )
    end
    utils.makePassages(location, rooms)
    utils.execTemplates(location, "DUNGEON", 1, 2)
    utils.fixOverlapped(location)
    utils.execTemplates(location, "DUNGEON", 2, 6)
    utils.placeWalls(location)
    utils.execTemplates(location, "DUNGEON", 6, 10)
    -- utils.placeStairs(location)
  end
);

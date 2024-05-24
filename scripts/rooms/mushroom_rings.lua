spec = {
  maps = {
    {
    " mmm ",
    "m...m",
    "m...m",
    " mmm ",
    },
    {
    " mm ",
    "m..m",
    "m..m",
    " mm ",
    },
  };
  floor = CellType.GROUND;
  rotate = true;
  cells = {
    [" "] = function(location, room, cell)
      cell.type = CellType.EMPTY;
    end;
    ["."] = function(location, room, cell)
      cell.tags:add("WIPE")
    end;
    ["m"] = function(location, room, cell)
        o = gm:create(location, "MUSHROOM", cell.x, cell.y, 0)
        room:addEntity(o.entity)
    end
  };
}
room = utils.makeTemplateFromSpec(spec)
templates.MUSHROOM_RINGS = room


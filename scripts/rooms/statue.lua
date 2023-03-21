spec = {
  map = {
    "...",
    ".&.",
    "...",
  };
  floor = CellType.FLOOR;
  rotate = false;
  cells = {
    ["."] = function(location, room, cell) end;
    ["&"] = function(location, room, cell)
      o = gm:create(location, "STATUE", cell.x, cell.y, 0)
      room:addEntity(o.entity)
    end;
  };
}
room = utils.makeTemplateFromSpec(spec)
templates.STATUE = room


f = utils.makeFeature(function(location)
  utils.scatter(location, "BUSH", 30, 70, false)

  cells = utils.scatter(location, "", 0, 7, false)
  for _, c in ipairs(cells) do
    r = utils.makeBlob(location, 8, 3, 8, 3, CellType.EMPTY, CellType.UNKNOWN, true, 2)
    for _, cell in ipairs(r.cells) do
      if (cell.type == CellType.EMPTY) then
        cell.x = c.x+cell.x
        cell.y = c.y+cell.y
        location:addEntity("BUSH", cell)
      end
    end
  end
end)
features.BUSHES = f


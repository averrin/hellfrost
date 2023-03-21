f = utils.makeFeature(function(location)
  cells = utils.scatter(location, "", 10, 30, false)
  for _, c in ipairs(cells) do
    c.type = CellType.WATER
  end
end)
features.PONDS = f




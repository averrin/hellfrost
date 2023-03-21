data = gm:getData()
rt = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 15, 8, 15, 8, CellType.WATER, CellType.UNKNOWN, true, 4)
  return r
end)
templates.LAKE = rt



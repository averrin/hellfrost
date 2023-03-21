data = gm:getData()
rt = utils.makeTemplate(function(location)
  r = utils.makeBlob(location, 15, 5, 15, 5, CellType.VOID, CellType.UNKNOWN, true, 2)
  return r
end)
templates.VOID_FIELD = rt

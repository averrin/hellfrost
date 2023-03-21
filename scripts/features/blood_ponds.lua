f = utils.makeFeature(function(location)
  cells = utils.scatter(location, "", 10, 50, false)
  for _, c in ipairs(cells) do
      c:addFeature(CellFeature.BLOOD);
  end
end)
features.BLOOD_PONDS = f




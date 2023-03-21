f = utils.makeFeature(function(location)
  cells = utils.scatter(location, "BONES", 2, 12, false)
  for _, c in ipairs(cells) do
    if (Random.bool(0.66)) then
      c:addFeature(CellFeature.BLOOD);
    end
  end
end)
features.BONES = f



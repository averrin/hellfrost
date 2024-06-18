data_file = "game.bin"
tileset = "boxy"
-- seed=1320999521
seed = 873130520
-- location_type="DUNGEON"
location_type = "MIX"
margin = 8

light = {
  alpha_per_d = 10,
  alpha_blend_inc = 0.5,
  flick_delay = 200,
  blend_mode = "blend",
  max_bright = 220,
}

-- hide all following vars into table
win_prefix = "meta"
alt_prefix = "alt"
ctrl_prefix = "ctrl"
shift_prefix = "shift"
key_delim = "+"

mapping = {
  ["J"] = function()
    movePlayer(Direction.S);
  end,
  ["H"] = function()
    movePlayer(Direction.W);
  end,
  ["K"] = function()
    movePlayer(Direction.N);
  end,
  ["L"] = function()
    movePlayer(Direction.E);
  end,
  ["U"] = function()
    movePlayer(Direction.NW);
  end,
};

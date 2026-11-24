-- SPDX-License-Identifier: 0BSD
-- HEPTADI-LUA3L-001: Lua 3.2.2. Only lexical locals, no reads or writes of globals.
-- No imports, libraries, files, network, callbacks, or external runtime installation.
local xor_byte = function(left, right)
  local value = 0
  local place = 1
  while left > 0 or right > 0 do
    local a = left % 2
    local b = right % 2
    if a ~= b then
      value = value + place
    end
    left = (left - a) / 2
    right = (right - b) / 2
    place = place * 2
  end
  return value
end

local verify = function()
  local byte = 0
  local laps = 0
  while byte <= 255 do
    local nested = xor_byte(150, xor_byte(105, byte))
    local independent = 255 - byte
    if nested ~= independent then
      return 0
    end
    if xor_byte(105, xor_byte(150, nested)) ~= byte then
      return 0
    end
    byte = byte + 1
    laps = laps + 1
  end
  local original = xor_byte(150, xor_byte(105, 219))
  local mutated = xor_byte(150, xor_byte(105, 223))
  -- OR-envelope collision is checked by arithmetic bit 0x04 here.
  if original ~= 36 or mutated ~= 32 or laps ~= 256 then
    return 0
  end
  return 1
end

local result = verify()
if result ~= 1 then
  local invalid = nil
  invalid.failure = 1
end
